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
	std::stack<SymbolT> GetStack();
	// Gets the item on top of the stack and removes it from the stack.
	SymbolT PopStack();
	// Reads symbols off the stack until ';' is encountered, and returns the symbols in reverse popping order.
	ProgramT PopProgram();
	// Pushes an item to the top of the stack.
	void PushStack(SymbolT item);

	// Gets a copy of the current queue
	std::queue<SymbolT> GetQueue();
	// Gets the item at the top of the queue and removes it from the queue.
	SymbolT Dequeue();
	// Puts an item at the back of the queue.
	void Enqueue(SymbolT item);

	// Makes a copy of all definitions currently in effect.
	SymbolMapT CopyDefinitions();

	// Executes a symbol in the current interpreter state
	void Interpret(SymbolT symbol);
	// Executes a symbol using the selected interpreter state
	void Interpret(SymbolT symbol, const SymbolMapT& state);

	// Redefines a symbol
	void Redefine(SymbolT symbol, std::shared_ptr<EmmentalDefinition> definition);

private:
	std::stack<SymbolT> ProgramStack;
	std::queue<SymbolT> ProgramQueue;

	SymbolMapT SymbolMap;

	void GenerateDefaultSymbols();

	EmmentalDefinition* GetDefinition(SymbolT symbol, const SymbolMapT& state);
};
