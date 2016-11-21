#ifndef _UTILS_H
#define _UTILS_H

#include <Windows.h>

void *DetourFunction( void *pLocation, void *pDetour, size_t ulLength );

#endif