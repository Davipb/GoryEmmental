#pragma once
#include "NativeDefinition.h"
#include "InterpretedDefinition.h"
#include "Emmental.h"

namespace Util
{
	enum class ConsoleColor
	{
		Black = 0, Red, Green, Yellow, Blue, Magenta, Cyan, White, Extended, Default,
		BrightBlack = 60, BrightRed, BrightGreen, BrightYellow, BrightBlue, BrightMagenta, BrightCyan, BrightWhite
	};

	std::string GetPrintable(SymbolT symbol);
	void DescribeSymbol(SymbolT symbol, std::ostream& output, bool align = false);
	void DescribeMemory(const Emmental& interpreter, std::ostream& output);
	void DescribeDefinitions(const SymbolMapT& map, std::ostream& output);
	void DescribeDefinition(SymbolT symbol, const SymbolMapT& map, bool full, std::ostream& output, bool align = false);
	void DescribeDefinition(SymbolT symbol, const EmmentalDefinition* const definition, bool full, std::ostream& output, bool align = false);
	void DescribeProgram(const ProgramT& program, std::ostream& output);
	void Colorize(ConsoleColor foreground, std::ostream& output);
	void Colorize(ConsoleColor foreground, ConsoleColor background, std::ostream& output);
}