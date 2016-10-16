#include "Emmental.h"

Emmental::Emmental(std::basic_istream<SymbolType>* const inputStream, std::basic_ostream<SymbolType>* const outputStream)
	: InputStream(inputStream), OutputStream(outputStream)
{
	
}

Emmental::~Emmental()
{
	for (auto& symbol : SymbolMap)
	{
		delete symbol.second;
		symbol.second = nullptr;
	}

	SymbolMap.clear();
}

SymbolType Emmental::PopStack()
{
	SymbolType result = ProgramStack.top();
	ProgramStack.pop();

	return result;
}

void Emmental::PushStack(SymbolType item)
{
	ProgramStack.push(item);
}

SymbolType Emmental::Dequeue()
{
	SymbolType result = ProgramQueue.front();
	ProgramQueue.pop();

	return result;
}

void Emmental::Enqueue(SymbolType item)
{
	ProgramQueue.push(item);
}

EmmentalDefinition* Emmental::GetDefinition(SymbolType symbol)
{
	auto result = SymbolMap.find(symbol);

	if (result == SymbolMap.end())
		return nullptr;

	return result->second;
}

std::map<SymbolType, EmmentalDefinition*>& Emmental::CopyDefinitions()
{
	return std::map<SymbolType, EmmentalDefinition*>(SymbolMap);
}

void Emmental::GenerateDefaultSymbols()
{
	// TODO
}
