#include "StdAfx.h"
#include "VMInfo.h"

char szLogBuf[2048] ;

CVMInfo::CVMInfo(void)
{
}

CVMInfo::~CVMInfo(void)
{
}

VOID CVMInfo::VM_GetSysInfo()
{
	GetSystemInfo ( &this->SysInfo ) ;
}

VOID CVMInfo::VM_GetMemStatus()
{
	GlobalMemoryStatus ( &this->MemStatus ) ;
}

BOOL CVMInfo::VM_GetMemInfoList ( DWORD dwProcessId )
{
	// ��������ڴ���Ϣ����
	this->MemList.clear () ;

	HANDLE hProcess = INVALID_HANDLE_VALUE;
	if ( dwProcessId != GetCurrentProcessId() )
	{
		// ��Ŀ�����
		hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,dwProcessId );
		if ( hProcess == NULL )
			return FALSE ;
	}

	MEMORY_BASIC_INFORMATION	MemBaseInfo ;
	DWORD dwInfoSize = sizeof(MEMORY_BASIC_INFORMATION) ;
	DWORD dwCurPos = (DWORD)this->SysInfo.lpMinimumApplicationAddress ;
	while ( dwCurPos < (DWORD)this->SysInfo.lpMaximumApplicationAddress )
	{
		// ��ѯָ�����̵�ָ����ַ��״̬��Ϣ
		VirtualQueryEx ( hProcess, (LPVOID)dwCurPos, &MemBaseInfo, dwInfoSize ) ;
		// ��״̬��Ϣ��ӵ�����
		this->MemList.push_back(MemBaseInfo) ;
		// ��λ����һ������
		dwCurPos = (DWORD)MemBaseInfo.BaseAddress + MemBaseInfo.RegionSize ;
	}
	return TRUE ;
}

UINT CVMInfo::VM_FirstSearch ( DWORD dwProcessId, DWORD dwValue )
{
	if ( this->MemList.empty () )
		return 0;

	this->AddrList.clear () ;

	HANDLE hProcess = INVALID_HANDLE_VALUE;
	if ( dwProcessId != GetCurrentProcessId() )
	{
		// ��Ŀ�����
		hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,dwProcessId );
		if ( hProcess == NULL )
			return 0 ;
	}

	LPBYTE lpBuf = new BYTE[this->SysInfo.dwPageSize] ;	// ҳ�滺����
	list<MEMORY_BASIC_INFORMATION>::iterator p ;
	// �����ڴ�����
	for ( p = this->MemList.begin(); p != this->MemList.end(); p++ )
	{
		// ����Ƿ�Ϊ���ύ��״̬
		if ( p->State != MEM_COMMIT )
			continue ;

		sprintf ( szLogBuf, "Base=0x%08X Size=0x%08X", p->BaseAddress, p->RegionSize ) ;
		OutputDebugStringA ( szLogBuf ) ;

		DWORD dwSize = 0, dwReadBytes = 0  ;
		// ������ǰ�ڴ�����������ҳ��
		while ( dwSize < p->RegionSize )
		{
			LPVOID lpAddr = (LPVOID)((DWORD)p->BaseAddress+dwSize) ;
			if ( ReadProcessMemory ( hProcess, lpAddr, lpBuf, this->SysInfo.dwPageSize, &dwReadBytes ) == FALSE )
			{
				dwSize += this->SysInfo.dwPageSize ;
				continue ;
			}

			// �ڵ�ǰҳ������Ŀ��ֵ
			for ( int i = 0; i < this->SysInfo.dwPageSize-3; i++ )
			{
				// ��Ŀ���ַ��ӵ���ַ������
				if ( *((DWORD*)(lpBuf+i)) == dwValue )
					this->AddrList.push_back ( (DWORD)lpAddr+i ) ;

				// ������������Ŀ���ַ��Ŀ̫��ʱ����������
				if ( this->AddrList.size() == MAX_ADDR_NUM )
				{
					delete []lpBuf ;
					CloseHandle ( hProcess ) ;
					return MAX_ADDR_NUM ;
				}
			}

			dwSize += this->SysInfo.dwPageSize ;
		}
	}

	delete []lpBuf ;
	CloseHandle ( hProcess ) ;
	return this->AddrList.size () ;
}

UINT CVMInfo::VM_ReSearch ( DWORD dwProcessId, DWORD dwValue )
{
	HANDLE hProcess = INVALID_HANDLE_VALUE;
	if ( dwProcessId != GetCurrentProcessId() )
	{
		// ��Ŀ�����
		hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,dwProcessId );
		if ( hProcess == NULL )
			return FALSE ;
	}

	list<DWORD> TempList ;
	list<DWORD>::iterator p ;
	DWORD dwReadBytes = 0, dwCurValue = 0 ;
	for ( p = this->AddrList.begin(); p != this->AddrList.end (); p++ )
	{
		ReadProcessMemory ( hProcess, (LPVOID)*p, &dwCurValue, sizeof(DWORD), &dwReadBytes ) ;
		if ( dwCurValue == dwValue )
			TempList.push_back ( *p ) ;
	}
	this->AddrList.clear () ;
	this->AddrList.insert ( this->AddrList.begin(), TempList.begin(), TempList.end() ) ;

	CloseHandle ( hProcess ) ;
	return this->AddrList.size() ;
}

BOOL CVMInfo::VM_Modify ( DWORD dwProcessId, DWORD dwTagAddr, DWORD dwValue )
{
	HANDLE hProcess = INVALID_HANDLE_VALUE;
	if ( dwProcessId != GetCurrentProcessId() )
	{
		// ��Ŀ�����
		hProcess = OpenProcess( PROCESS_ALL_ACCESS,FALSE,dwProcessId );
		if ( hProcess == NULL )
			return FALSE ;
	}

	DWORD dwWriteBytes = 0, dwOldProtect ;
	// �޸�Ŀ���ַ�ı�����ʽΪ�ɶ���д
	VirtualProtectEx ( hProcess, (LPVOID)dwTagAddr, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect ) ; 
	// �޸�����
	WriteProcessMemory ( hProcess, (LPVOID)dwTagAddr, &dwValue, sizeof(DWORD), &dwWriteBytes ) ;
	// ��ԭĿ���ַ�ı�����ʽ
	VirtualProtectEx ( hProcess, (LPVOID)dwTagAddr, sizeof(DWORD), dwOldProtect, NULL ) ; 

	CloseHandle ( hProcess ) ;
	return TRUE ;
}