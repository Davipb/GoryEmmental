#pragma once
#include <istream>
#include <stack>
#include <queue>
#include <map>
#include "Config.h"

class Emmental
{
public:
	// Creates a new Emmental interpreter with a specified IO Streams
	// Note: The interpreter does not take ownership of the stream. It must be deleted manually.
	Emmental(std::basic_istream<SymbolType>* const inputStream, std::basic_ostream<SymbolType>* const outputStream);
	~Emmental();

	// Gets the item on top of the stack and removes it from the stack.
	SymbolType PopStack();
	// Pushes an item to the top of the stack.
	void PushStack(SymbolType item);

	// Gets the item at the top of the queue and removes it from the queue.
	SymbolType Dequeue();
	// Puts an item at the back of the queue.
	void Enqueue(SymbolType item);

	// Gets the current definition of a symbol.
	class EmmentalDefinition* GetDefinition(SymbolType symbol);
	// Makes a copy of all definitions currently in effect.
	std::map<SymbolType, EmmentalDefinition*>& CopyDefinitions();

private:
	std::basic_istream<SymbolType>* const InputStream;
	std::basic_ostream<SymbolType>* const OutputStream;

	std::stack<SymbolType> ProgramStack;
	std::queue<SymbolType> ProgramQueue;

	std::map<SymbolType, EmmentalDefinition*> SymbolMap;

	void GenerateDefaultSymbols();
};
