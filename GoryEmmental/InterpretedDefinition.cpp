#include "InterpretedDefinition.h"
#include "Emmental.h"

InterpretedDefinition::InterpretedDefinition(const ProgramT& program, const SymbolMapT& state)
	: Program(program), CapturedState(state)
{
}

void InterpretedDefinition::Execute(Emmental* interpreter, std::size_t recursionLevel)
{
	for (SymbolT symbol : Program)
	{
		interpreter->Interpret(symbol, CapturedState, recursionLevel);
	}
}

ProgramT InterpretedDefinition::GetProgram() const { return Program; }

SymbolMapT InterpretedDefinition::GetDefinitions() const { return CapturedState; }
