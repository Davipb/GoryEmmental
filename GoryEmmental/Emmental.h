#pragma once
#include <istream>
#include <stack>
#include <queue>
#include <map>
#include <vector>
#include <memory>
#include "Config.h"
#include "NativeDefinition.h"

class Emmental
{
public:
	std::istream& InputStream;
	std::ostream& OutputStream;

	// Creates a new Emmental interpreter with a specified IO Streams
	// Note: The interpreter does not take ownership of the stream. It must be deleted manually.
	Emmental(std::istream& inputStream, std::ostream& outputStream);

	// Gets a copy of the current stack
	std::stack<SymbolType> GetStack();
	// Gets the item on top of the stack and removes it from the stack.
	SymbolType PopStack();
	// Reads symbols off the stack until ';' is encountered, and returns the symbols in reverse popping order.
	std::vector<SymbolType> PopProgram();
	// Pushes an item to the top of the stack.
	void PushStack(SymbolType item);

	// Gets a copy of the current queue
	std::queue<SymbolType> GetQueue();
	// Gets the item at the top of the queue and removes it from the queue.
	SymbolType Dequeue();
	// Puts an item at the back of the queue.
	void Enqueue(SymbolType item);

	// Makes a copy of all definitions currently in effect.
	std::map<SymbolType, std::shared_ptr<EmmentalDefinition>> CopyDefinitions();

	// Executes a symbol in the current interpreter state
	void Interpret(SymbolType symbol);
	// Executes a symbol using the selected interpreter state
	void Interpret(SymbolType symbol, const std::map<SymbolType, std::shared_ptr<EmmentalDefinition>>& state);

	// Redefines a symbol
	void Redefine(SymbolType symbol, std::shared_ptr<EmmentalDefinition> definition);

private:
	std::stack<SymbolType> ProgramStack;
	std::queue<SymbolType> ProgramQueue;

	std::map<SymbolType, std::shared_ptr<EmmentalDefinition>> SymbolMap;

	void GenerateDefaultSymbols();

	EmmentalDefinition* GetDefinition(SymbolType symbol, const std::map<SymbolType, std::shared_ptr<EmmentalDefinition>>& state);
};
