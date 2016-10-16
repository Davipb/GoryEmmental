#pragma once
#include <istream>
#include <stack>
#include <queue>
#include <map>
#include <vector>
#include "Config.h"
#include "NativeDefinition.h"

class Emmental
{
public:
	std::basic_istream<SymbolType>* const InputStream;
	std::basic_ostream<SymbolType>* const OutputStream;

	// Creates a new Emmental interpreter with a specified IO Streams
	// Note: The interpreter does not take ownership of the stream. It must be deleted manually.
	Emmental(std::basic_istream<SymbolType>* const inputStream, std::basic_ostream<SymbolType>* const outputStream);
	~Emmental();

	// Gets the item on top of the stack and removes it from the stack.
	SymbolType PopStack();
	// Reads symbols off the stack until ';' is encountered, and returns the symbols in reverse popping order.
	std::vector<SymbolType> PopProgram();
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

	void Interpret(SymbolType symbol);

private:
	std::stack<SymbolType> ProgramStack;
	std::queue<SymbolType> ProgramQueue;

	std::map<SymbolType, EmmentalDefinition*> SymbolMap;

	void GenerateDefaultSymbols();
};
