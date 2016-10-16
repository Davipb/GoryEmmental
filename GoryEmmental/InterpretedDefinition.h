#pragma once
#include <vector>
#include <map>
#include <memory>
#include "Config.h"
#include "EmmentalDefinition.h"

class InterpretedDefinition :
	public EmmentalDefinition
{
public:
	InterpretedDefinition(const std::vector<SymbolT>& program, const std::map<SymbolT, std::shared_ptr<EmmentalDefinition>>& state);
	void Execute(Emmental* interpreter) override;

private:
	std::vector<SymbolT> Program;
	std::map<SymbolT, std::shared_ptr<EmmentalDefinition>> CapturedState;
};

