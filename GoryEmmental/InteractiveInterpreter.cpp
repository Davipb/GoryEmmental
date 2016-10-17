#include <iomanip>
#include "InteractiveInterpreter.h"
#include "InterpretedDefinition.h"
#include "Util.h"
#include "Globals.h"

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

		if (Globals::DebugMode)
		{
			Util::DescribeMemory(*Interpreter, Interpreter->OutputStream);
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

	AddCommand(InteractiveCommand("debug", "Toggles debug mode on/off", [](Emmental* interpreter, std::string)
	{
		Globals::DebugMode = !Globals::DebugMode;
		interpreter->OutputStream << "Debug mode is now " << (Globals::DebugMode ? "on" : "off") << "." << std::endl;
	}));

	AddCommand(InteractiveCommand("color", "Toggles Virtual Console coloring on/off", [](Emmental* interpreter, std::string)
	{
		Globals::UseVirtualConsole = !Globals::UseVirtualConsole;
		interpreter->OutputStream << "Virtual Console coloring is now " << (Globals::UseVirtualConsole ? "on" : "off") << "." << std::endl;
	}));

	AddCommand(InteractiveCommand("optimize", "Toggles program optimization on/off", [](Emmental* interpreter, std::string)
	{
		Globals::OptimizeProgram = !Globals::OptimizeProgram;
		interpreter->OutputStream << "Optimization is now " << (Globals::OptimizeProgram ? "on" : "off") << "." << std::endl;
	}));

	AddCommand(InteractiveCommand("memory", "Shows the current stack and queue", [](Emmental* interpreter, std::string)
	{
		Util::DescribeMemory(*interpreter, interpreter->OutputStream);
		interpreter->OutputStream << std::endl;
	}));

	AddCommand(InteractiveCommand("pushraw",
		"Pushes its argument into the stack (interpreted as raw ASCII bytes). Note: Whitespace directly after the command is also interpreted as part of the argument!",
		[](Emmental* interpreter, std::string args)
	{
		for (auto& symbol : args)
			interpreter->Push(symbol);

		interpreter->OutputStream << "Pushed " << args.length() << " bytes into the stack.";
	}));

	AddCommand(InteractiveCommand("defs", 
		"Without argument: Displays all current symbol definitions. With symbol number as argument: Displays all captured definitions for the symbol.",
		[](Emmental* interpreter, std::string arg)
	{
		if (arg.empty())
		{
			interpreter->OutputStream << "Current interpreter definitions: " << std::endl;
			Util::DescribeDefinitions(interpreter->CopyDefinitions(), interpreter->OutputStream);
			interpreter->OutputStream << std::endl;
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

			Util::DescribeDefinition(symbol, interpreter->CopyDefinitions(), true, interpreter->OutputStream);
			interpreter->OutputStream << std::endl;
		}
	}));

	AddCommand(InteractiveCommand("info", "Display interpreter information.", [](Emmental* interpreter, std::string)
	{
		interpreter->OutputStream << "Gory Emmental Interpreter 1.0.0 by Davipb" << std::endl;

		Util::Colorize(Util::ConsoleColor::BrightGreen, interpreter->OutputStream);
		interpreter->OutputStream << "== Compile-Time Settings ==" << std::endl;
		Util::Colorize(Util::ConsoleColor::Default, interpreter->OutputStream);
		interpreter->OutputStream << "Symbol Type: " << typeid(SymbolT).name() << std::endl;
		interpreter->OutputStream << "Symbol Size: " << sizeof(SymbolT) << " byte(s)" << std::endl;
		interpreter->OutputStream << "Max Stack Size: " << EMMENTAL_MAX_STACK_SIZE << std::endl;
		interpreter->OutputStream << "Max Queue Size: " << EMMENTAL_MAX_QUEUE_SIZE << std::endl;
		interpreter->OutputStream << "Max Recursion Level: " << EMMENTAL_MAX_RECURSION_LEVEL << std::endl;

		Util::Colorize(Util::ConsoleColor::BrightGreen, interpreter->OutputStream);
		interpreter->OutputStream << "== Runtime Settings ==" << std::endl;
		Util::Colorize(Util::ConsoleColor::Default, interpreter->OutputStream);
		interpreter->OutputStream << "Debug Mode: " << (Globals::DebugMode ? "On" : "Off") << std::endl;
		interpreter->OutputStream << "Colors: " << (Globals::UseVirtualConsole ? "On" : "Off") << std::endl;
		interpreter->OutputStream << "Optimization: " << (Globals::OptimizeProgram ? "On" : "Off") << std::endl;

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
		Interpreter->OutputStream << std::endl;

		for (auto& x : Commands)
		{
			Util::Colorize(Util::ConsoleColor::BrightGreen, Interpreter->OutputStream);
			Interpreter->OutputStream << "__" << x.GetName() << ": ";
			Util::Colorize(Util::ConsoleColor::Default, Interpreter->OutputStream);
			Interpreter->OutputStream << x.GetDescription() << std::endl;
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
		
	Util::Colorize(Util::ConsoleColor::Red, Interpreter->OutputStream);
	Interpreter->OutputStream << "Unknown command. Use __help for a list of commands." << std::endl;
	Util::Colorize(Util::ConsoleColor::Default, Interpreter->OutputStream);

	return true;
}