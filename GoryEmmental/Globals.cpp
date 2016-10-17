#include "Globals.h"

#if _WIN32
#include <Windows.h>
#endif

bool Globals::DebugMode = false;
bool Globals::UseVirtualConsole = false;

void Globals::Initialize()
{

#if _WIN32

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
	// Target Windows SDK doesn't support Virtual Terminal, default to not using
	UseVirtualConsole = false
#else // !ENABLE_VIRTUAL_TERMINAL_PROCESSING

	// Check if Windows version supports Virtual Console color
	// Windows 10 Anniversary Edition and up should support it

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE || hOut == nullptr)
	{
		// Unknown output device, don't use colors by default
		UseVirtualConsole = false;
	}
	else
	{
		DWORD dwMode = 0;
		if (!GetConsoleMode(hOut, &dwMode))
		{
			// Unable to get console mode, default to no colors
			UseVirtualConsole = false;
		}
		else
		{
			dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			// Use Virtual Console only if enabling it was a success
			UseVirtualConsole = SetConsoleMode(hOut, dwMode);		
		}

	}
#endif // !ENABLE_VIRTUAL_TERMINAL_PROCESSING

#else // _WIN32
	// Unknown system, default to no color.
	UseVirtualConsole = false;
#endif // _WIN32
}
