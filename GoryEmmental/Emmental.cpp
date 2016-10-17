#include "Emmental.h"
#include "NativeDefinition.h"
#include "InterpretedDefinition.h"

Emmental::Emmental(std::istream& inputStream, std::ostream& outputStream, std::ostream& errorStream)
	: InputStream(inputStream), OutputStream(outputStream), ErrorStream(errorStream)
{
	GenerateDefaultSymbols();
}

std::stack<SymbolT> Emmental::GetStack() const
{
	return ProgramStack;
}

SymbolT Emmental::PopSymbol()
{
	if (ProgramStack.empty())
	{
		ErrorStream << "Error: Tried popping symbol from empty stack. Returning default value." << std::endl;
		return SymbolT();
	}

	SymbolT result = ProgramStack.top();
	ProgramStack.pop();

	return result;
}

ProgramT Emmental::PopProgram()
{
	if (ProgramStack.empty())
	{
		ErrorStream << "Error: Tried popping program from empty stack. Returning default program." << std::endl;
		return ProgramT();
	}

	ProgramT result;
	SymbolT symbol = PopSymbol();
	
	while (symbol != ';')
	{
		result.push_back(symbol);

		if (ProgramStack.empty())
		{
			ErrorStream << "Error: Stack ran out before ';' was found to terminate a program. Returning incomplete program." << std::endl;
			break;
		}
		else
		{
			symbol = PopSymbol();
		}
	}

	std::reverse(result.begin(), result.end());
	return result;
}

void Emmental::Push(SymbolT item)
{
	if (ProgramStack.size() >= EMMENTAL_MAX_STACK_SIZE)
	{
		ErrorStream << "Error: Tried to push symbol to full stack. Ignoring." << std::endl;
		return;
	}

	ProgramStack.push(item);
}

void Emmental::ClearStack()
{
	while (!ProgramStack.empty())
		ProgramStack.pop();
}

std::queue<SymbolT> Emmental::GetQueue() const
{
	return ProgramQueue;
}

SymbolT Emmental::Dequeue()
{
	if (ProgramQueue.empty())
	{
		ErrorStream << "Error: Tried dequeuing symbol from empty queue. Returning default value." << std::endl;
		return SymbolT();
	}

	SymbolT result = ProgramQueue.front();
	ProgramQueue.pop();

	return result;
}

void Emmental::Enqueue(SymbolT item)
{
	if (ProgramQueue.size() >= EMMENTAL_MAX_QUEUE_SIZE)
	{
		ErrorStream << "Error: Tried to enqueue symbol to full queue. Ignoring." << std::endl;
		return;
	}

	ProgramQueue.push(item);
}

void Emmental::ClearQueue()
{
	while (!ProgramQueue.empty())
		ProgramQueue.pop();
}

EmmentalDefinition* Emmental::GetDefinition(SymbolT symbol) const { return GetDefinition(symbol, SymbolMap); }

SymbolMapT Emmental::CopyDefinitions() const { return SymbolMap; }

SymbolMapT Emmental::CopyDefinitions(ProgramT program) const
{
	if (program.empty())
		return SymbolMapT();

	SymbolMapT result;
	auto end = std::unique(program.begin(), program.end());

	for (auto it = program.begin(); it != end; ++it)
	{
		auto definition = SymbolMap.find(*it);

		if (definition != SymbolMap.end())
			result[*it] = definition->second;
	}

	return result;
}

void Emmental::ResetDefinitions()
{
	SymbolMap.clear();
	GenerateDefaultSymbols();
}

void Emmental::Interpret(SymbolT symbol) { Interpret(symbol, SymbolMap); }
void Emmental::Interpret(SymbolT symbol, std::size_t recursionLevel) { Interpret(symbol, SymbolMap, recursionLevel); }
void Emmental::Interpret(SymbolT symbol, const SymbolMapT& state) { Interpret(symbol, state, 0); }

void Emmental::Interpret(SymbolT symbol, const SymbolMapT& state, std::size_t recursionLevel)
{
	if (recursionLevel >= EMMENTAL_MAX_RECURSION_LEVEL)
	{
		ErrorStream << "Error: Recursion level too high. Ignoring symbol '" << symbol << "'." << std::endl;
		return;
	}

	EmmentalDefinition* definition = GetDefinition(symbol, state);

	if (definition)
		definition->Execute(this, recursionLevel + 1);
	else
		ErrorStream << "Warning: Tried to interpret undefined symbol '" << symbol << "'. Ignoring symbol." << std::endl;
}

void Emmental::Redefine(SymbolT symbol, std::shared_ptr<EmmentalDefinition> definition)
{
	if (definition)
		SymbolMap[symbol] = definition;
	else
		ErrorStream << "Error: Tried to redefine symbol '" << symbol << "' with a null definition." << std::endl;
}

void Emmental::Reset()
{
	ClearStack();
	ClearQueue();
	ResetDefinitions();
}

void Emmental::GenerateDefaultSymbols()
{
	// Push NULL to the stack
	SymbolMap['#'] = std::make_shared<NativeDefinition>([](Emmental* interpreter, std::size_t) { interpreter->Push(0); });

	// 0 through 9 pop a stack symbol, multiply it by ten, add themselves to the multiplied number and push the result to the stack.
	for (SymbolT i = 0; i <= 9; i++)
	{
		SymbolMap['0' + i] = std::make_shared<NativeDefinition>([i](Emmental* interpreter, std::size_t)
		{
			SymbolT popped = interpreter->PopSymbol();
			interpreter->Push(i + popped * 10);
		});
	}

	// Add two stack symbols and push result to stack
	SymbolMap['+'] = std::make_shared<NativeDefinition>([](Emmental* interpreter, std::size_t)
	{ 
		interpreter->Push(interpreter->PopSymbol() + interpreter->PopSymbol()); 
	});
	// Subtract first from second stack symbol and push result to stack
	SymbolMap['-'] = std::make_shared<NativeDefinition>([](Emmental* interpreter, std::size_t)
	{ 
		SymbolT first = interpreter->PopSymbol();
		SymbolT second = interpreter->PopSymbol();

		interpreter->Push(second - first); 
	});
	// Push discrete log2 (highest set bit) of stack symbol (0 is treated as 256)
	SymbolMap['~'] = std::make_shared<NativeDefinition>([](Emmental* interpreter, std::size_t)
	{ 
		SymbolT symbol = interpreter->PopSymbol();
		SymbolT log2;

		if (symbol == 0)
			log2 = 8;
		else
			log2 = (SymbolT)std::log2(symbol);

		interpreter->Push(log2); 
	});
	// Enqueue top stack symbol (doesn't remove it from the stack)
	SymbolMap['^'] = std::make_shared<NativeDefinition>([](Emmental* interpreter, std::size_t)
	{
		SymbolT symbol = interpreter->PopSymbol();
		interpreter->Enqueue(symbol);
		interpreter->Push(symbol);
	});
	// Dequeue to stack
	SymbolMap['V'] = std::make_shared<NativeDefinition>([](Emmental* interpreter, std::size_t)
	{
		SymbolT symbol = interpreter->Dequeue();
		interpreter->Push(symbol);
	});
	// Duplicate front queue symbol
	SymbolMap[':'] = std::make_shared<NativeDefinition>([](Emmental* interpreter, std::size_t)
	{
		SymbolT symbol = interpreter->PopSymbol();
		interpreter->Push(symbol);
		interpreter->Push(symbol);
	});
	// Pop stack to output
	SymbolMap['.'] = std::make_shared<NativeDefinition>([](Emmental* interpreter, std::size_t)
	{
		SymbolT symbol = interpreter->PopSymbol();
		interpreter->OutputStream << symbol;
	});
	// Get input symbol and push to stack
	SymbolMap[','] = std::make_shared<NativeDefinition>([](Emmental* interpreter, std::size_t)
	{
		SymbolT symbol;
		interpreter->InputStream >> symbol;
		interpreter->Push(symbol);
	});
	// For convenience, ';' puts ';' on the stack.
	SymbolMap[';'] = std::make_shared<NativeDefinition>([](Emmental* interpreter, std::size_t) { interpreter->Push(';'); });
	// Eval: Interpret the top stack symbol
	SymbolMap['?'] = std::make_shared<NativeDefinition>([](Emmental* interpreter, std::size_t recursionLevel)
	{
		SymbolT symbol = interpreter->PopSymbol();
		interpreter->Interpret(symbol, recursionLevel);
	});
	
	// This is the main command of the Emmental: Supplant.
	// Pop a symbol and a program from the stack. Redefine the symbol as the popped program.
	SymbolMap['!'] = std::make_shared<NativeDefinition>([](Emmental* interpreter, std::size_t)
	{
		SymbolT symbol = interpreter->PopSymbol();
		ProgramT program = interpreter->PopProgram();
		SymbolMapT state = interpreter->CopyDefinitions(program);

		interpreter->Redefine(
			symbol, 
			std::make_shared<InterpretedDefinition>(program, state)
		);

	});
}

EmmentalDefinition* Emmental::GetDefinition(SymbolT symbol, const SymbolMapT& state) const
{
	auto result = state.find(symbol);

	if (result == state.end())
		return nullptr;

	return result->second.get();
}