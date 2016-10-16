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
	InterpretedDefinition(const std::vector<SymbolType>& program, const std::map<SymbolType, std::shared_ptr<EmmentalDefinition>>& state);
	void Execute(Emmental* interpreter) override;

private:
	std::vector<SymbolType> Program;
	std::map<SymbolType, std::shared_ptr<EmmentalDefinition>> CapturedState;
};

