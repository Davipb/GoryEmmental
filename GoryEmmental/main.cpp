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

	while (!file.eof())
	{
		SymbolT symbol;
		file >> symbol;
		interpreter.Interpret(symbol);

		if (Globals::DebugMode)
		{
			std::cout << std::endl;

			std::cout << "Symbol: ";
			Util::DescribeSymbol(symbol, std::cout);
			std::cout << std::endl;

			Util::DescribeMemory(interpreter, std::cout);
		}
	}

	file.close();
	return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
	try
	{
		TCLAP::CmdLine cmd("Gory Emmental is a C++ interpreter for the esoteric language Emmental.", ' ', "1.0.0");
		
		TCLAP::SwitchArg debugModeArg("d", "debug", "Show Stack and Queue after each symbol.");
		TCLAP::SwitchArg colorArg("c", "color", "Use virtual console terminal sequences to colorize interpreter output.");
		TCLAP::SwitchArg interactiveModeArg("i", "interactive", "Read symbols from the console.");
		TCLAP::UnlabeledValueArg<std::string> inputFileArg("Input", "The Emmental code file to interpret.", true, "", "file", false);

		cmd.add(debugModeArg);
		cmd.xorAdd(interactiveModeArg, inputFileArg);

		cmd.parse(argc, argv);
		Globals::Initialize();

		if (debugModeArg.isSet())
			Globals::DebugMode = debugModeArg.getValue();

		if (colorArg.isSet())
			Globals::UseVirtualConsole = colorArg.getValue();

		if (interactiveModeArg.isSet())
		{
			Emmental interpreter(std::cin, std::cout, std::cerr);
			InteractiveInterpreter interactive(&interpreter);
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