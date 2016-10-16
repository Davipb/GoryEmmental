#include <iostream>
#include <string>
#include <fstream>
#include "Emmental.h"
#include "InterpretedDefinition.h"
#include "InteractiveInterpreter.h"
#include "tclap\CmdLine.h"

int InterpretFile(std::string filename, bool debug)
{
	Emmental interpreter(std::cin, std::cout, std::cerr);
	std::basic_ifstream<SymbolT> file(filename, std::ios_base::in);

	while (!file.eof())
	{
		SymbolT symbol;
		file >> symbol;
		interpreter.Interpret(symbol);

		if (debug)
		{
			std::cout << std::endl;
			std::cout << "Symbol: " << std::to_string(symbol) << " '" << symbol << "'" << std::endl;
			std::cout << "Stack: ";
			std::stack<SymbolT> stack = interpreter.GetStack();
			while (!stack.empty())
			{
				std::cout << std::to_string(stack.top()) << " '" << stack.top() << "'; ";
				stack.pop();
			}
			std::cout << std::endl;

			std::cout << "Queue: ";
			std::queue<SymbolT> queue = interpreter.GetQueue();
			while (!queue.empty())
			{
				std::cout << std::to_string(queue.front()) << " '" << queue.front() << "'; ";
				queue.pop();
			}
			std::cout << std::endl;
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
		TCLAP::SwitchArg interactiveModeArg("i", "interactive", "Read symbols from the console.");
		TCLAP::UnlabeledValueArg<std::string> inputFileArg("Input", "The Emmental code file to interpret.", true, "", "file", false);

		cmd.add(debugModeArg);
		cmd.xorAdd(interactiveModeArg, inputFileArg);

		cmd.parse(argc, argv);
		bool debug = debugModeArg.getValue();

		if (interactiveModeArg.isSet())
		{
			Emmental interpreter(std::cin, std::cout, std::cerr);
			InteractiveInterpreter interactive(&interpreter);
			interactive.DebugMode = debug;
			return interactive.RunLoop();
		}

		return InterpretFile(inputFileArg.getValue(), debug);
	}
	catch (TCLAP::ArgException& e)
	{
		std::cerr << "Error: " << e.error() << " for argument " << e.argId() << std::endl;
		return EXIT_FAILURE;
	}
}