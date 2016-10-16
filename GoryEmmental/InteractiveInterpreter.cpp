#include "InteractiveInterpreter.h"
#include "InterpretedDefinition.h"
#include <iomanip>

InteractiveInterpreter::InteractiveInterpreter(Emmental* const interpreter)
	: Interpreter(interpreter)
{
	GenerateCommands();
}

int InteractiveInterpreter::RunLoop()
{
	Interpreter->OutputStream << "Entering Interactive Mode" << std::endl;
	Interpreter->OutputStream << "Inputs not starting with '__' will be interpreted by Emmental." << std::endl;
	Interpreter->OutputStream << "Type __help for a list of commands, or __exit to exit." << std::endl;


	while (true)
	{
		Interpreter->OutputStream << std::endl;
		Interpreter->OutputStream << "> ";
		std::string input;
		std::getline(Interpreter->InputStream, input);

		// Special command to exit the loop
		if (input.find("__exit") == 0)
			return EXIT_SUCCESS;
		
		if (ParseCommand(input))
			continue;

		for (auto&& x : input)
		{
			SymbolT symbol = (SymbolT)x;
			Interpreter->Interpret(symbol);
		}

		if (DebugMode)
		{
			auto command = std::find_if(Commands.begin(), Commands.end(), [](InteractiveCommand x) { return x.GetName() == "memory"; });

			if (command != Commands.end())
				command->Execute(Interpreter, std::string());
		}
	}
}

void InteractiveInterpreter::AddCommand(const InteractiveCommand& command) { Commands.push_back(command); }

void InteractiveInterpreter::GenerateCommands()
{
	// These commands are handled as special cases, they'll never be called normally.
	AddCommand(InteractiveCommand("exit", "Exits this program.", nullptr));
	AddCommand(InteractiveCommand("help", "Shows this list.", nullptr));

	// Normal commands here
	AddCommand(InteractiveCommand("reset", "Resets the interpreter back to its original state.", [](Emmental* interpreter, std::string)
	{
		interpreter->Reset();
		interpreter->OutputStream << "Interpreter reset." << std::endl;
	}));

	AddCommand(InteractiveCommand("clearstack", "Clears the stack.", [](Emmental* interpreter, std::string)
	{
		interpreter->ClearStack();
		interpreter->OutputStream << "Stack cleared." << std::endl;
	}));

	AddCommand(InteractiveCommand("clearqueue", "Clears the queue.", [](Emmental* interpreter, std::string)
	{
		interpreter->ClearQueue();
		interpreter->OutputStream << "Queue cleared." << std::endl;
	}));

	AddCommand(InteractiveCommand("resetdefs", "Resets all symbol definitions back to their original native definitions.", [](Emmental* interpreter, std::string)
	{
		interpreter->ResetDefinitions();
		interpreter->OutputStream << "Symbol definitions reset to original native definitions." << std::endl;
	}));

	AddCommand(InteractiveCommand("debug", "Toggles debug mode on/off", [&](Emmental* interpreter, std::string)
	{
		DebugMode = !DebugMode;
		interpreter->OutputStream << "Debug mode is now " << (DebugMode ? "on" : "off") << "." << std::endl;
	}));

	AddCommand(InteractiveCommand("memory", "Shows the current stack and queue", [&](Emmental* interpreter, std::string)
	{
		Interpreter->OutputStream << "Stack: ";
		std::stack<SymbolT> stack = Interpreter->GetStack();
		while (!stack.empty())
		{
			Interpreter->OutputStream << std::to_string(stack.top()) << " '" << stack.top() << "'; ";
			stack.pop();
		}
		Interpreter->OutputStream << std::endl;

		Interpreter->OutputStream << "Queue: ";
		std::queue<SymbolT> queue = Interpreter->GetQueue();
		while (!queue.empty())
		{
			Interpreter->OutputStream << std::to_string(queue.front()) << " '" << queue.front() << "'; ";
			queue.pop();
		}
		Interpreter->OutputStream << std::endl;
	}));

	AddCommand(InteractiveCommand("defs", 
		"Without argument: Displays all current symbol definitions.\nWith symbol number as argument: Displays all captured definitions for the symbol.",
		[&](Emmental* interpreter, std::string arg)
	{
		if (arg.empty())
		{
			interpreter->OutputStream << "Current interpreter definitions: " << std::endl;
			DisplayMap(interpreter->CopyDefinitions(), interpreter->OutputStream);
		}
		else
		{
			SymbolT symbol;

			try
			{
				symbol = std::stoi(arg);
			}
			catch (std::invalid_argument)
			{
				interpreter->OutputStream << "Invalid symbol" << std::endl;
				return;
			}
			catch (std::out_of_range)
			{
				interpreter->OutputStream << "Symbol value out of range." << std::endl;
				return;
			}

			interpreter->OutputStream << std::setw(3) << std::to_string(symbol) << " '" << symbol << "' -> ";

			EmmentalDefinition* definition = interpreter->GetDefinition(symbol);
			InterpretedDefinition* interpreted = dynamic_cast<InterpretedDefinition*>(definition);

			if (interpreted == nullptr)
			{
				interpreter->OutputStream << "(Native)" << std::endl;
				return;
			}
			
			for (auto&& x : interpreted->GetProgram())
				interpreter->OutputStream << x;

			interpreter->OutputStream << std::endl;
			interpreter->OutputStream << "Captured state: " << std::endl;

			DisplayMap(interpreted->GetDefinitions(), interpreter->OutputStream);			
		}
	}));

	AddCommand(InteractiveCommand("info", "Display interpreter information.", [](Emmental* interpreter, std::string)
	{
		interpreter->OutputStream << "Gory Emmental Interpreter 1.0.0 by Davipb" << std::endl;
		interpreter->OutputStream << "Symbol Type: " << typeid(SymbolT).name() << std::endl;
		interpreter->OutputStream << "Symbol Size: " << sizeof(SymbolT) << " byte(s)" << std::endl;
		interpreter->OutputStream << "Max Stack Size: " << EMMENTAL_MAX_STACK_SIZE << std::endl;
		interpreter->OutputStream << "Max Queue Size: " << EMMENTAL_MAX_QUEUE_SIZE << std::endl;
		interpreter->OutputStream << "Max Recursion Level: " << EMMENTAL_MAX_RECURSION_LEVEL << std::endl;
	}));
}

bool InteractiveInterpreter::ParseCommand(std::string input)
{
	if (input.find("__") != 0)
		return false;

	std::string command = input.substr(2, input.size() - 2);

	if (command.find("help") == 0)
	{
		Interpreter->OutputStream << "Commands are case-sensitive. Arguments come right after a command." << std::endl;
		for (auto& x : Commands)
		{
			Interpreter->OutputStream << "__" << x.GetName() << std::endl;
			Interpreter->OutputStream << x.GetDescription() << std::endl;
			Interpreter->OutputStream << std::endl;

		}
		return true;
	}

	for (auto& x : Commands)
	{
		if (command.find(x.GetName()) == 0)
		{
			x.Execute(Interpreter, command.substr(x.GetName().size(), command.size() - x.GetName().size()));
			return true;
		}
	}

	Interpreter->OutputStream << "Unknown command. Use __help for a list of commands." << std::endl;
	return true;
}

void InteractiveInterpreter::DisplayMap(const SymbolMapT& map, std::ostream& output)
{
	for (auto&& pair : map)
	{
		output << std::setw(3) << std::to_string(pair.first) << " '" << pair.first << "'";
		output << " -> ";

		InterpretedDefinition* interpreted = dynamic_cast<InterpretedDefinition*>(pair.second.get());

		if (interpreted)
		{
			for (auto&& x : interpreted->GetProgram())
				output << x;
		}
		else
		{
			output << "(Native)";
		}

		output << std::endl;
	}
}