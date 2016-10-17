#include "Util.h"
#include <string>
#include <iomanip>
#include "Globals.h"

static const Util::ConsoleColor SingleSymbolColor = Util::ConsoleColor::Green;
static const Util::ConsoleColor MultiSymbolColor = Util::ConsoleColor::BrightCyan;
static const Util::ConsoleColor MultiSymbolAltColor = Util::ConsoleColor::BrightYellow;
static const Util::ConsoleColor UndefinedColor = Util::ConsoleColor::Red;
static const Util::ConsoleColor NativeDefinitionColor = Util::ConsoleColor::BrightBlue;

std::string Util::GetPrintable(SymbolT symbol)
{
	switch (symbol)
	{
	case 0: return "NUL";
	case 1: return "SOH";
	case 2: return "STX";
	case 3: return "ETX";
	case 4: return "EOT";
	case 5: return "ENQ";
	case 6: return "ACK";
	case 7: return "BEL";
	case 8: return "BS";
	case 9: return "TAB";
	case 10: return "LF";
	case 11: return "VT";
	case 12: return "FF";
	case 13: return "CR";
	case 14: return "SO";
	case 15: return "SI";
	case 16: return "DLE";
	case 17: return "DC1";
	case 18: return "DC2";
	case 19: return "DC3";
	case 20: return "DC4";
	case 21: return "NAK";
	case 22: return "SYN";
	case 23: return "ETB";
	case 24: return "CAN";
	case 25: return "EM";
	case 26: return "SUB";
	case 27: return "ESC";
	case 28: return "FS";
	case 29: return "GS";
	case 30: return "RS";
	case 31: return "US";
	case 32: return "SP";
	case 127: return "DEL";
	default: return std::string(1, symbol);
	}
}

void Util::DescribeSymbol(SymbolT symbol, std::ostream& output, bool align)
{
	if (align)
		output << std::setfill('0') << std::setw(3) << std::to_string(symbol);
	else
		output << std::to_string(symbol);

	output << " '";

	std::string printable = GetPrintable(symbol);
	if (printable.length() > 1)
		Colorize(MultiSymbolColor, output);
	else
		Colorize(SingleSymbolColor, output);

	output << printable;

	Colorize(ConsoleColor::Default, output);
	output << "'";

	if (align)
		output << std::setfill(' ') << std::setw(3 - printable.size()) << "";
}

void Util::DescribeMemory(const Emmental& interpreter, std::ostream& output)
{
	output << "Stack: ";
	std::stack<SymbolT> stack = interpreter.GetStack();
	while (!stack.empty())
	{
		DescribeSymbol(stack.top(), output);
		output << ", ";
		stack.pop();
	}
	output << std::endl;

	output << "Queue: ";
	std::queue<SymbolT> queue = interpreter.GetQueue();
	while (!queue.empty())
	{
		DescribeSymbol(stack.top(), output);
		output << ", ";
		queue.pop();
	}
}

void Util::DescribeDefinitions(const SymbolMapT& map, std::ostream& output)
{	
	for (auto& pair : map)
	{
		DescribeDefinition(pair.first, pair.second.get(), false, output, true);		
		output << std::endl;
	}

	output << std::to_string(map.size()) << " definitions";
}

void Util::DescribeDefinition(SymbolT symbol, const SymbolMapT& map, bool full, std::ostream& output, bool align)
{
	auto result = map.find(symbol);

	if (result == map.end())
	{
		Util::DescribeSymbol(symbol, output, align);

		Colorize(UndefinedColor, output);
		output << " [Undefined]";
		Colorize(ConsoleColor::Default, output);
	}
	else
	{
		DescribeDefinition(symbol, result->second.get(), full, output, align);
	}
}

void Util::DescribeDefinition(SymbolT symbol, const EmmentalDefinition* const definition, bool full, std::ostream& output, bool align)
{
	Util::DescribeSymbol(symbol, output, align);

	if (definition == nullptr)
	{
		Colorize(UndefinedColor, output);
		output << " [Undefined]";
		Colorize(ConsoleColor::Default, output);
		return;
	}

	output << " -> ";	

	const InterpretedDefinition* const interpreted = dynamic_cast<const InterpretedDefinition* const>(definition);
	if (interpreted)
	{
		DescribeProgram(interpreted->GetProgram(), output);

		if (full)
		{			
			output << std::endl;
			output << "Captured definitions for symbol: " << std::endl;
			DescribeDefinitions(interpreted->GetDefinitions(), output);
		}
	}
	else
	{
		Colorize(NativeDefinitionColor, output);
		output << "(Native)";
		Colorize(ConsoleColor::Default, output);


		if (full)
		{
			output << std::endl;
			output << "Native definitions have no captures.";
		}
	}
}

void Util::DescribeProgram(const ProgramT& program, std::ostream& output)
{
	bool multiAlt = false;
	bool lastWasMulti = false;
	for (auto&& x : program)
	{
		std::string printable = GetPrintable(x);

		if (printable.length() > 1)
		{
			lastWasMulti = true;
			Colorize(multiAlt ? MultiSymbolAltColor : MultiSymbolColor, output);
			output << printable;
			Colorize(ConsoleColor::Default, output);
			multiAlt = !multiAlt;
		}
		else
		{
			lastWasMulti = multiAlt = false;
			Colorize(SingleSymbolColor, output);
			output << printable;
			Colorize(ConsoleColor::Default, output);
		}
	}
}

#define CSI "\x1b["
void Util::Colorize(ConsoleColor foreground, std::ostream & output)
{
	if (!Globals::UseVirtualConsole)
		return;

	output << CSI;
	output << std::to_string(30 + (int)foreground);
	output << "m";
}

void Util::Colorize(ConsoleColor foreground, ConsoleColor background, std::ostream & output)
{
	if (!Globals::UseVirtualConsole)
		return;

	if (foreground == ConsoleColor::Default && background == ConsoleColor::Default)
	{
		output << CSI << "0" << "m";
	}
	else
	{
		output << CSI;
		output << std::to_string(30 + (int)foreground);
		output << ";";
		output << std::to_string(40 + (int)background);
		output << "m";
	}
}
#undef CSI