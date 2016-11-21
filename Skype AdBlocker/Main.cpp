#include "Utils.h"

typedef BOOL( __stdcall *tSetWindowPos )( HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags );

tSetWindowPos oSetWindowPos = nullptr;
uintptr_t ulSetBannerRetnAddr = 0;

BOOL __stdcall hkSetWindowPos( HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags )
{
	char szClassName[32] = { 0 };
	RealGetWindowClassA( hWnd, szClassName, 31 );

	if( strcmp( szClassName, "TChatBanner" ) == 0 )
	{
		// we set width and height of this banner to 0
		return oSetWindowPos( hWnd, hWndInsertAfter, X, Y, 0, 0, uFlags );
	}

	return oSetWindowPos( hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags );
}

DWORD WINAPI MainThread( LPVOID lpParameter )
{
	// For Windows 10 there is a function called win32u!NtUserSetWindowPos, for older OSes you can use user32!SetWindowPos

	FARPROC lpSetWindowPos = nullptr;

	HMODULE hUser = GetModuleHandleW( L"win32u.dll" );
	if( hUser != nullptr )
	{
		lpSetWindowPos = GetProcAddress( hUser, "NtUserSetWindowPos" );
	}
	else
	{
		hUser = GetModuleHandleW( L"user32.dll" );
		if( hUser != nullptr )
		{
			lpSetWindowPos = GetProcAddress( hUser, "SetWindowPos" );
		}
	}

	if( lpSetWindowPos )
	{
		oSetWindowPos = reinterpret_cast< tSetWindowPos >( DetourFunction( lpSetWindowPos, hkSetWindowPos, 5 ) );
	}
	else
	{
		MessageBoxW( NULL, L"Something went wrong :/", L"Shit", MB_OK | MB_ICONERROR );
	}

	return EXIT_SUCCESS;
}

BOOL WINAPI DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
	if( dwReason == DLL_PROCESS_ATTACH )
	{
		CreateThread( NULL, 0, MainThread, NULL, 0, NULL );
	}

	return TRUE;
}