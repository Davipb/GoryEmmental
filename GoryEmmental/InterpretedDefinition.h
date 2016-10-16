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
	InterpretedDefinition(const ProgramT& program, const SymbolMapT& state);
	void Execute(Emmental* interpreter, std::size_t recursionLevel) override;

	ProgramT GetProgram() const;
	SymbolMapT GetDefinitions() const;

private:
	ProgramT Program;
	SymbolMapT CapturedState;
};

