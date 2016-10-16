#include <iostream>
#include <string>
#include "Emmental.h"
#include "InterpretedDefinition.h"
#include "InteractiveInterpreter.h"
#include "tclap\CmdLine.h"

int main(int argc, char** argv)
{
	try
	{
		TCLAP::CmdLine cmd("Gory Emmental is a C++ interpreter for the esoteric language Emmental.", ' ', "1.0.0");
		
		TCLAP::SwitchArg debugModeArg("d", "debug", "Show Stack and Queue after each symbol.");
		TCLAP::SwitchArg interactiveModeArg("i", "interactive", "Read symbols from the console.");
		TCLAP::UnlabeledValueArg<std::string> inputFileArg("Input", "The Emmental code file to interpret.", true, "", "file", false);

		cmd.add(debugModeArg);
		cmd.xorAdd(interactiveModeArg, inputFileArg);

		cmd.parse(argc, argv);

		if (interactiveModeArg.isSet())
		{
			Emmental interpreter(std::cin, std::cout, std::cerr);
			InteractiveInterpreter interactive(&interpreter);
			interactive.DebugMode = debugModeArg.getValue();
			return interactive.RunLoop();
		}

		return EXIT_SUCCESS;
	}
	catch (TCLAP::ArgException& e)
	{
		std::cerr << "Error: " << e.error() << " for argument " << e.argId() << std::endl;
		return EXIT_FAILURE;
	}
}