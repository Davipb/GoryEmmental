#include "Emmental.h"
#include "NativeDefinition.h"
#include "InterpretedDefinition.h"

Emmental::Emmental(std::istream& inputStream, std::ostream& outputStream)
	: InputStream(inputStream), OutputStream(outputStream)
{
	GenerateDefaultSymbols();
}

std::stack<SymbolT> Emmental::GetStack()
{
	return ProgramStack;
}

SymbolT Emmental::PopStack()
{
	SymbolT result = ProgramStack.top();
	ProgramStack.pop();

	return result;
}

ProgramT Emmental::PopProgram()
{
	ProgramT result;
	SymbolT symbol = PopStack();
	
	while (symbol != ';')
	{
		result.push_back(symbol);
		symbol = PopStack();
	}

	std::reverse(result.begin(), result.end());
	return result;
}

void Emmental::PushStack(SymbolT item)
{
	ProgramStack.push(item);
}

std::queue<SymbolT> Emmental::GetQueue()
{
	return ProgramQueue;
}

SymbolT Emmental::Dequeue()
{
	SymbolT result = ProgramQueue.front();
	ProgramQueue.pop();

	return result;
}

void Emmental::Enqueue(SymbolT item)
{
	ProgramQueue.push(item);
}

SymbolMapT Emmental::CopyDefinitions()
{
	return SymbolMap;
}

void Emmental::Interpret(SymbolT symbol) { Interpret(symbol, SymbolMap); }

void Emmental::Interpret(SymbolT symbol, const SymbolMapT& state)
{
	EmmentalDefinition* definition = GetDefinition(symbol, state);

	if (definition)
		definition->Execute(this);
}

void Emmental::Redefine(SymbolT symbol, std::shared_ptr<EmmentalDefinition> definition)
{
	SymbolMap[symbol] = definition;
}

void Emmental::GenerateDefaultSymbols()
{
	// Push NULL to the stack
	SymbolMap['#'] = std::make_shared<NativeDefinition>([](Emmental* interpreter) { interpreter->PushStack(0); });

	// 0 through 9 pop a stack symbol, multiply it by ten, add themselves to the multiplied number and push the result to the stack.
	for (SymbolT i = 0; i <= 9; i++)
	{
		SymbolMap['0' + i] = std::make_shared<NativeDefinition>([i](Emmental* interpreter)
		{
			SymbolT popped = interpreter->PopStack();
			interpreter->PushStack(i + popped * 10);
		});
	}

	// Add two stack symbols and push result to stack
	SymbolMap['+'] = std::make_shared<NativeDefinition>([](Emmental* interpreter) { interpreter->PushStack(interpreter->PopStack() + interpreter->PopStack()); });
	// Subtract first from second stack symbol and push result to stack
	SymbolMap['-'] = std::make_shared<NativeDefinition>([](Emmental* interpreter) 
	{ 
		SymbolT first = interpreter->PopStack();
		SymbolT second = interpreter->PopStack();

		interpreter->PushStack(second - first); 
	});
	// Push discrete log2 (highest set bit) of stack symbol (0 is treated as 256)
	SymbolMap['~'] = std::make_shared<NativeDefinition>([](Emmental* interpreter) 
	{ 
		SymbolT symbol = interpreter->PopStack();
		SymbolT log2;

		if (symbol == 0)
			log2 = 8;
		else
			log2 = (SymbolT)std::log2(symbol);

		interpreter->PushStack(log2); 
	});
	// Enqueue top stack symbol (doesn't remove it from the stack)
	SymbolMap['^'] = std::make_shared<NativeDefinition>([](Emmental* interpreter)
	{
		SymbolT symbol = interpreter->PopStack();
		interpreter->Enqueue(symbol);
		interpreter->PushStack(symbol);
	});
	// Dequeue to stack
	SymbolMap['V'] = std::make_shared<NativeDefinition>([](Emmental* interpreter)
	{
		SymbolT symbol = interpreter->Dequeue();
		interpreter->PushStack(symbol);
	});
	// Duplicate front queue symbol
	SymbolMap[':'] = std::make_shared<NativeDefinition>([](Emmental* interpreter)
	{
		SymbolT symbol = interpreter->PopStack();
		interpreter->PushStack(symbol);
		interpreter->PushStack(symbol);
	});
	// Pop stack to output
	SymbolMap['.'] = std::make_shared<NativeDefinition>([](Emmental* interpreter)
	{
		SymbolT symbol = interpreter->PopStack();
		interpreter->OutputStream << symbol;
	});
	// Get input symbol and push to stack
	SymbolMap[','] = std::make_shared<NativeDefinition>([](Emmental* interpreter)
	{
		SymbolT symbol;
		interpreter->InputStream >> symbol;
		interpreter->PushStack(symbol);
	});
	// For convenience, ';' puts ';' on the stack.
	SymbolMap[';'] = std::make_shared<NativeDefinition>([](Emmental* interpreter) { interpreter->PushStack(';'); });
	// Eval: Interpret the top stack symbol
	SymbolMap['?'] = std::make_shared<NativeDefinition>([](Emmental* interpreter)
	{
		SymbolT symbol = interpreter->PopStack();
		interpreter->Interpret(symbol);
	});
	
	// This is the main command of the Emmental: Supplant.
	// Pop a symbol and a program from the stack. Redefine the symbol as the popped program.
	SymbolMap['!'] = std::make_shared<NativeDefinition>([](Emmental* interpreter)
	{
		SymbolT symbol = interpreter->PopStack();
		ProgramT program = interpreter->PopProgram();

		interpreter->Redefine(
			symbol, 
			std::make_shared<InterpretedDefinition>(program, interpreter->CopyDefinitions())
		);

	});
}

EmmentalDefinition* Emmental::GetDefinition(SymbolT symbol, const SymbolMapT& state)
{
	auto result = state.find(symbol);

	if (result == state.end())
		return nullptr;

	return result->second.get();
}