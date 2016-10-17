#include "Util.h"
#include <string>
#include <iomanip>

std::string Util::GetPrintable(SymbolT symbol)
{
	// Printable ASCII characters can be converted directly
	if (symbol > 31 && symbol != 127)
		return std::string(1, symbol);

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
	case 127: return "DEL";
	default: return "???";
	}
}

void Util::DescribeSymbol(SymbolT symbol, std::ostream& output)
{
	output << std::setfill('0') << std::setw(3) << std::to_string(symbol) << " '" << GetPrintable(symbol) << "'";
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
		DescribeDefinition(pair.first, pair.second.get(), false, output);		
		output << std::endl;
	}

	output << std::to_string(map.size()) << " definitions";
}

void Util::DescribeDefinition(SymbolT symbol, const SymbolMapT& map, bool full, std::ostream& output)
{
	auto result = map.find(symbol);

	if (result == map.end())
	{
		Util::DescribeSymbol(symbol, output);
		output << " [Undefined]";
	}
	else
	{
		DescribeDefinition(symbol, result->second.get(), full, output);
	}
}

void Util::DescribeDefinition(SymbolT symbol, const EmmentalDefinition* const definition, bool full, std::ostream& output)
{
	Util::DescribeSymbol(symbol, output);

	if (definition == nullptr)
	{
		output << " [Undefined]";
		return;
	}

	output << " -> ";	

	const InterpretedDefinition* const interpreted = dynamic_cast<const InterpretedDefinition* const>(definition);
	if (interpreted)
	{
		for (auto&& x : interpreted->GetProgram())
			output << x;

		if (full)
		{			
			output << std::endl;
			output << "Captured definitions for symbol: " << std::endl;
			DescribeDefinitions(interpreted->GetDefinitions(), output);
		}
	}
	else
	{
		output << "(Native)";

		if (full)
		{
			output << std::endl;
			output << "Native definitions have no captures.";
		}
	}
}
