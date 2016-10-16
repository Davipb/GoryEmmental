#include "InterpretedDefinition.h"
#include "Emmental.h"

InterpretedDefinition::InterpretedDefinition(const std::vector<SymbolT>& program, const std::map<SymbolT, std::shared_ptr<EmmentalDefinition>>& state)
	: Program(program), CapturedState(state)
{
}

void InterpretedDefinition::Execute(Emmental* interpreter)
{
	for (SymbolT symbol : Program)
	{
		interpreter->Interpret(symbol, CapturedState);
	}
}
