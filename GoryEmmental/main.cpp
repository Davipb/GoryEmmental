#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include "Emmental.h"
#include "InterpretedDefinition.h"
#include "InteractiveInterpreter.h"
#include "Util.h"
#include "Globals.h"
#include "tclap\CmdLine.h"

int InterpretFile(const std::string& filename)
{
	Emmental interpreter(std::cin, std::cout, std::cerr);	

#if _WIN32 && _UNICODE
	// Open file using UTF16 filename
	// Non-Standard MSVC extension, allows usage of std::wstring for Unicode filenames
	std::basic_ifstream<SymbolT> file(Util::ToUtf16(filename), std::ios_base::binary | std::ios_base::in);
#else // _WIN32 && _UNICODE
	std::basic_ifstream<SymbolT> file(filename, std::ios_base::binary | std::ios_base::in);
#endif // _WIN32 && _UNICODE
	
	try
	{
		file.exceptions(decltype(file)::failbit);

		SymbolT symbol;
		while (file.get(symbol))
		{
			if (Globals::IgnoreWhitespace && std::isspace(symbol))
				continue;

			interpreter.Interpret(symbol);

			if (Globals::DebugMode && !Globals::QuietMode)
			{
				std::cout << std::endl;
				std::cout << "Interpreted Symbol: ";
				Util::DescribeSymbol(symbol, std::cout);
				std::cout << std::endl;
				Util::DescribeMemory(interpreter, std::cout);
				std::cout << std::endl;
			}
		}

		file.close();
	}
	catch (const decltype(file)::failure& fail)
	{
		if (!file.eof())
		{
			if (!Globals::QuietMode)
				std::cerr << "Error " << fail.code() << " while trying to read file: " << fail.what() << std::endl;

			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

int Start(std::vector<std::string>& args)
{
	Globals::Initialize();

	try
	{
		TCLAP::CmdLine cmd("Gory Emmental is a C++ interpreter for the esoteric language Emmental.", '=', "1.0.0");
		
		TCLAP::SwitchArg debugModeArg("d", "debug", "Shows Stack and Queue after each symbol.", cmd, Globals::DebugMode);
		TCLAP::SwitchArg colorArg("c", "color", 
			"Disables Virtual Console coloring for systems that support it, or forcefully enables it for systems that don't.", 
			cmd, Globals::UseVirtualConsole);
		TCLAP::SwitchArg optimizeArg("o", "optimize", 
			"Bypasses some formal language definitions to make programs more efficient without altering their behavior.", 
			cmd, Globals::OptimizeProgram);
		TCLAP::SwitchArg ignoreWhitespaceArg("w", "nowhitespace", "Ignores whitespace characters in the Emmental program.", cmd, Globals::IgnoreWhitespace);
		TCLAP::SwitchArg quietArg("q", "quiet", "Only prints program output.", cmd, Globals::QuietMode);
		TCLAP::SwitchArg lenientArg("l", "lenient", "Treats execution errors as warnings and uses non-standard interpreter behavior to continue program execution.", 
			cmd, Globals::LenientMode);

		TCLAP::SwitchArg interactiveModeArg("i", "interactive", "Uses interactive mode.", false);
		TCLAP::UnlabeledValueArg<std::string> inputFileArg("Input", "The Emmental code file to interpret.", true, "", "file", false);
		cmd.xorAdd(interactiveModeArg, inputFileArg);

		cmd.parse(args);
		Globals::DebugMode = debugModeArg.getValue();
		Globals::UseVirtualConsole = colorArg.getValue();
		Globals::OptimizeProgram = optimizeArg.getValue();
		Globals::IgnoreWhitespace = ignoreWhitespaceArg.getValue();
		Globals::QuietMode = quietArg.getValue();
		Globals::LenientMode = lenientArg.getValue();

		if (interactiveModeArg.isSet())
		{
			Emmental interpreter(std::cin, std::cout, std::cerr);
			InteractiveInterpreter interactive(interpreter);
			return interactive.RunLoop();
		}

		return InterpretFile(inputFileArg.getValue());
	}
	catch (TCLAP::ArgException& e)
	{
		std::cerr << "Error: " << e.error() << " for argument " << e.argId() << std::endl;
		return EXIT_FAILURE;
	}
}

#if _WIN32 && _UNICODE
int wmain(int argc, wchar_t** argv)
{
	// Convert from UTF16 to UTF8
	std::vector<std::string> args;
	for (int i = 0; i < argc; i++)
	{
		args.emplace_back(Util::ToUtf8(argv[i]));
	}

	return Start(args);
}
#else // _WIN32 && _UNICODE
int main(int argc, char** argv)
{
	// Put arguments in a vector
	std::vector<std::string> args;
	for (int i = 0; i < argc; i++)
	{
		args.emplace_back(argv[i]);
	}

	return Start(args);
}
#endif // _WIN32 && _UNICODE