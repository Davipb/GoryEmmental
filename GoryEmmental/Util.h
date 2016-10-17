#pragma once
#include "NativeDefinition.h"
#include "InterpretedDefinition.h"
#include "Emmental.h"

namespace Util
{
	std::string GetPrintable(SymbolT symbol);
	void DescribeSymbol(SymbolT symbol, std::ostream& output);
	void DescribeMemory(const Emmental& interpreter, std::ostream& output);
	void DescribeDefinitions(const SymbolMapT& map, std::ostream& output);
	void DescribeDefinition(SymbolT symbol, const SymbolMapT& map, bool full, std::ostream& output);
	void DescribeDefinition(SymbolT symbol, const EmmentalDefinition* const definition, bool full, std::ostream& output);
}