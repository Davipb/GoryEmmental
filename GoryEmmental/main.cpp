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

int InterpretFile(std::string filename)
{
	Emmental interpreter(std::cin, std::cout, std::cerr);
	std::basic_ifstream<SymbolT> file(filename, std::ios_base::in | std::ios_base::binary);

	if (file.fail() || file.bad())
	{
		std::cerr << "Couldn't open file.";
		return EXIT_FAILURE;
	}

	while (!file.eof())
	{
		int got = file.get();
		if (got == -1)
			continue;

		SymbolT symbol = got;

		if (Globals::IgnoreWhitespace &&std::isspace(symbol))
			continue;

		interpreter.Interpret(symbol);

		if (Globals::DebugMode)
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
	return EXIT_SUCCESS;
}

int main(int argc, char** argv)
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

		cmd.parse(argc, argv);
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