#pragma once
#include <functional>
#include <list>
#include <string>
#include "Emmental.h"

class InteractiveInterpreter final
{
public:
	struct InteractiveCommand
	{
	public:
		InteractiveCommand(const std::string& name, const std::string& description, const std::function<void(Emmental*, std::string)>& executor)
			: Name(name), Description(description), Executor(executor)
		{ }
		std::string GetName() const { return Name; }
		std::string GetDescription() const { return Description; }		
		void Execute(Emmental* interpreter, std::string args) { if (Executor) Executor(interpreter, args); }
	private:
		std::string Name;
		std::string Description;
		std::function<void(Emmental*, std::string)> Executor;
	};

public:
	bool DebugMode = false;

	InteractiveInterpreter(Emmental* const interpreter);
	int RunLoop();
	void AddCommand(const InteractiveCommand& command);

private:
	Emmental* const Interpreter;
	std::list<InteractiveCommand> Commands;
	void GenerateCommands();
	bool ParseCommand(std::string input);

	void DisplayMap(const SymbolMapT& map, std::ostream& output);

};

