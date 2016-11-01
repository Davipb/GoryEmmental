#include "Globals.h"

#if _WIN32
#	include <Windows.h>
#	ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#		error "Windows SDK outdated. Please use version 10 or higher."
#	endif // !ENABLE_VIRTUAL_TERMINAL_PROCESSING
#endif

bool Globals::DebugMode = false;
bool Globals::UseVirtualConsole = false;
bool Globals::OptimizeProgram = false;
bool Globals::IgnoreWhitespace = false;
bool Globals::QuietMode = false;
bool Globals::LenientMode = false;

#if _WIN32
static bool TryEnableWin32Color()
{
	// Check if Windows version supports Virtual Console color
	// Windows 10 Anniversary Edition and up should support it

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE || hOut == nullptr)
	{
		// Unknown output device, don't use colors by default
		return false;
	}

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))
	{
		// Unable to get console mode, default to no colors
		return false;
	}

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	// Use Virtual Console only if enabling it was a success
	return SetConsoleMode(hOut, dwMode) != 0;
}
#endif

void Globals::Initialize()
{
#if _WIN32
	UseVirtualConsole = TryEnableWin32Color();
#endif
}
