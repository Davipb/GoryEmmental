#include <iostream>
#include <string>
#include "Emmental.h"
#include "InterpretedDefinition.h"
#include "tclap\CmdLine.h"

void DisplayMap(const SymbolMapT& map)
{
	for (auto&& pair : map)
	{
		std::cout << std::to_string(pair.first) << " (" << pair.first << ")";
		std::cout << " -> ";

		InterpretedDefinition* interpreted = dynamic_cast<InterpretedDefinition*>(pair.second.get());

		if (interpreted)
		{
			for (auto&& x : interpreted->GetProgram())
				std::cout << x;
		}
		else
		{
			std::cout << "(Native)";
		}

		std::cout << std::endl;
	}
}

int Interactive(bool debug)
{
	std::cout << "Entering Gory Emmental Interactive mode. " << std::endl;
	std::cout << "Inputs not starting with '__' will be interpreted and ran as Emmental programs." << std::endl;
	std::cout << "The interpreter state persists between inputs." << std::endl;
	std::cout << "Type __exit to exit, or __help for other commands" << std::endl;
	Emmental interpreter(std::cin, std::cout, std::cerr);

	while (true)
	{
		std::cout << std::endl;
		std::cout << "> ";
		std::string input;
		std::getline(std::cin, input);

		if (input.find("__") == 0)
		{
			// Input begins with '__', use as a command

			std::string command = input.substr(2, input.length() - 2);

			if (command == "exit")
				return EXIT_SUCCESS;

			if (command == "reset")
			{
				interpreter.Reset();
				std::cout << "Interpreter state restored to default values" << std::endl;
				continue;
			}

			if (command.find("defs") == 0)
			{
				if (command.length() <= 5)
				{
					// No argument, display current interpreter definitions
					std::cout << "Current interpreter definitions: " << std::endl;
					DisplayMap(interpreter.CopyDefinitions());
				}
				else
				{
					SymbolT symbol = std::stoi(command.substr(5, command.length() - 5));
					EmmentalDefinition* definition = interpreter.GetDefinition(symbol);
					
					if (!definition)
					{
						std::cout << "Symbol " << std::to_string(symbol) << " (" << symbol << ") is not defined." << std::endl;
						continue;
					}

					InterpretedDefinition* program = dynamic_cast<InterpretedDefinition*>(definition);

					if (!program)
					{
						std::cout << "Symbol " << std::to_string(symbol) << " (" << symbol << ") is native, it has no captured state." << std::endl;
						continue;
					}

					std::cout << "Symbol " << std::to_string(symbol) << " (" << symbol << ")" << std::endl;
					std::cout << "Definition: ";

					for (auto&& x : program->GetProgram())
						std::cout << x;

					std::cout << std::endl;
					std::cout << "Captured definitions for symbol: " << std::endl;

					DisplayMap(program->GetDefinitions());					
				}

				continue;
			}

			if (command == "help")
			{
				// TODO
				std::cout << "Not Implemented" << std::endl;
				continue;
			}

			std::cout << "Unknown command. Type __exit to exit or __help for a list of commands." << std::endl;
		}

		for (auto&& x : input)
		{
			SymbolT symbol = (SymbolT)x;
			interpreter.Interpret(symbol);
		}

		if (debug)
		{
			std::cout << "Stack: ";
			std::stack<SymbolT> stack = interpreter.GetStack();
			while (!stack.empty())
			{
				std::cout << std::to_string(stack.top()) << " (" << stack.top() << "), ";
				stack.pop();
			}

			std::cout << "Queue: ";
			std::queue<SymbolT> queue = interpreter.GetQueue();
			while (!stack.empty())
			{
				std::cout << std::to_string(queue.front()) << " (" << queue.front() << "), ";
				queue.pop();
			}
		}
	}

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

		if (interactiveModeArg.isSet())
		{
			return Interactive(debugModeArg.getValue());
		}

		return EXIT_SUCCESS;
	}
	catch (TCLAP::ArgException& e)
	{
		std::cerr << "Error: " << e.error() << " for argument " << e.argId() << std::endl;
		return EXIT_FAILURE;
	}
}