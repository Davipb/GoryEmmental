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
	std::ostream& ErrorStream;

	// Creates a new Emmental interpreter with a specified IO Streams
	Emmental(std::istream& inputStream, std::ostream& outputStream, std::ostream& errorStream);

	// Gets a copy of the current stack
	std::stack<SymbolT> GetStack() const;
	// Gets the item on top of the stack and removes it from the stack.
	SymbolT PopSymbol();
	// Reads symbols off the stack until ';' is encountered, and returns the symbols in reverse popping order.
	ProgramT PopProgram();
	// Pushes an item to the top of the stack.
	void Push(SymbolT item);
	// Pops all elements off the stack.
	void ClearStack();

	// Gets a copy of the current queue
	std::queue<SymbolT> GetQueue() const;
	// Gets the item at the top of the queue and removes it from the queue.
	SymbolT Dequeue();
	// Puts an item at the back of the queue.
	void Enqueue(SymbolT item);
	// Dequeues all elements from the queue
	void ClearQueue();

	// Gets the current definition of a symbol. Returns nullptr if not defined.
	EmmentalDefinition* GetDefinition(SymbolT symbol) const;
	// Makes a copy of all current definitions
	SymbolMapT CopyDefinitions() const;
	// Makes a copy of all current definitions used in a program.
	SymbolMapT CopyDefinitions(ProgramT program) const;
	// Restores all definitions to their default values
	void ResetDefinitions();

	// Executes a symbol using the current interpreter state
	void Interpret(SymbolT symbol);
	// Executes a symbol using the current interpreter state and the selected recursion level
	void Interpret(SymbolT symbol, std::size_t recursionLevel);
	// Executes a symbol using the selected interpreter state
	void Interpret(SymbolT symbol, const SymbolMapT& state);
	// Executes a symbol using the selected interpreter state and the selected recursion level
	void Interpret(SymbolT symbol, const SymbolMapT& state, std::size_t recursionLevel);

	// Redefines a symbol
	void Redefine(SymbolT symbol, std::shared_ptr<EmmentalDefinition> definition);

	// Resets definitions, clears the stack and clears the queue.
	void Reset();

private:
	std::stack<SymbolT> ProgramStack;
	std::queue<SymbolT> ProgramQueue;

	SymbolMapT SymbolMap;

	void GenerateDefaultSymbols();

	EmmentalDefinition* GetDefinition(SymbolT symbol, const SymbolMapT& state) const;
};
