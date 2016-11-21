#include "Utils.h"

void *DetourFunction( void *pLocation, void *pDetour, size_t ulLength )
{
	void *pTrampoline = VirtualAlloc( NULL, ulLength + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );

	memcpy( pTrampoline, pLocation, ulLength );

	*reinterpret_cast< BYTE* >( reinterpret_cast< DWORD >( pTrampoline ) + ulLength ) = 0xE9;
	*reinterpret_cast< DWORD* >( reinterpret_cast< DWORD >( pTrampoline ) + ulLength + 1 ) = reinterpret_cast< DWORD >( pLocation ) - ( reinterpret_cast< DWORD >( pTrampoline ) + ulLength );

	DWORD dwOldProtection = NULL;
	VirtualProtect( pLocation, ulLength, PAGE_EXECUTE_READWRITE, &dwOldProtection );

	*reinterpret_cast< BYTE* >( reinterpret_cast< DWORD >( pLocation ) ) = 0xE9;
	*reinterpret_cast< DWORD* >( reinterpret_cast< DWORD >( pLocation ) + 1 ) = reinterpret_cast< DWORD >( pDetour ) - ( reinterpret_cast< DWORD >( pLocation ) + 5 );

	for( size_t i = 5; i < ulLength; i++ )
		*reinterpret_cast< BYTE* >( reinterpret_cast< DWORD >( pLocation ) + i ) = 0x90;

	VirtualProtect( pLocation, ulLength, dwOldProtection, NULL );

	return pTrampoline;
}
