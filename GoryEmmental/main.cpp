#include <iostream>
#include <string>
#include <fstream>
#include "Emmental.h"
#include "InterpretedDefinition.h"
#include "InteractiveInterpreter.h"
#include "Util.h"
#include "Globals.h"
#include "tclap\CmdLine.h"

int InterpretFile(std::string filename)
{
	Emmental interpreter(std::cin, std::cout, std::cerr);
	std::basic_ifstream<SymbolT> file(filename, std::ios_base::in);

	if (file.fail() || file.bad())
	{
		std::cerr << "Couldn't open file.";
		return EXIT_FAILURE;
	}

	while (!file.eof())
	{
		SymbolT symbol;
		file >> symbol;
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
		
		TCLAP::SwitchArg debugModeArg("d", "debug", "Show Stack and Queue after each symbol.", Globals::DebugMode);
		TCLAP::SwitchArg colorArg("c", "color", 
			"Disable Virtual Console coloring for systems that support it, or forcefully enable it for systems that don't.", 
			Globals::UseVirtualConsole);
		TCLAP::SwitchArg optimizeArg("o", "optimize", 
			"Bypass some formal language definitions to make programs more efficient without altering their behavior.", 
			Globals::OptimizeProgram);
		TCLAP::SwitchArg interactiveModeArg("i", "interactive", "Use interactive mode.", false);
		TCLAP::UnlabeledValueArg<std::string> inputFileArg("Input", "The Emmental code file to interpret.", true, "", "file", false);

		cmd.add(debugModeArg);
		cmd.add(colorArg);
		cmd.add(optimizeArg);
		cmd.xorAdd(interactiveModeArg, inputFileArg);

		cmd.parse(argc, argv);
		Globals::DebugMode = debugModeArg.getValue();
		Globals::UseVirtualConsole = colorArg.getValue();
		Globals::OptimizeProgram = optimizeArg.getValue();

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