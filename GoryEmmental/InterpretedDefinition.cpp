#include "InterpretedDefinition.h"
#include "Emmental.h"

InterpretedDefinition::InterpretedDefinition(const std::vector<SymbolType>& program, const std::map<SymbolType, std::shared_ptr<EmmentalDefinition>>& state)
	: Program(program), CapturedState(state)
{
}

void InterpretedDefinition::Execute(Emmental* interpreter)
{
	for (SymbolType symbol : Program)
	{
		interpreter->Interpret(symbol, CapturedState);
	}
}
