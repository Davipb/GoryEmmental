#include "Emmental.h"
#include "NativeDefinition.h"
#include "InterpretedDefinition.h"

Emmental::Emmental(std::basic_istream<SymbolType>* const inputStream, std::basic_ostream<SymbolType>* const outputStream)
	: InputStream(inputStream), OutputStream(outputStream)
{
	
}

SymbolType Emmental::PopStack()
{
	SymbolType result = ProgramStack.top();
	ProgramStack.pop();

	return result;
}

std::vector<SymbolType> Emmental::PopProgram()
{
	std::vector<SymbolType> result;
	SymbolType symbol = PopStack();
	
	while (symbol != ';')
	{
		result.push_back(symbol);
	}

	std::reverse(result.begin(), result.end());
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

std::map<SymbolType, std::shared_ptr<EmmentalDefinition>> Emmental::CopyDefinitions()
{
	return SymbolMap;
}

void Emmental::Interpret(SymbolType symbol) { Interpret(symbol, SymbolMap); }

void Emmental::Interpret(SymbolType symbol, const std::map<SymbolType, std::shared_ptr<EmmentalDefinition>>& state)
{
	EmmentalDefinition* definition = GetDefinition(symbol, state);

	if (definition)
		definition->Execute(this);
}

void Emmental::Redefine(SymbolType symbol, std::shared_ptr<EmmentalDefinition> definition)
{
	SymbolMap[symbol] = definition;
}

void Emmental::GenerateDefaultSymbols()
{
	// Push NULL to the stack
	SymbolMap['#'] = std::make_shared<NativeDefinition>([](Emmental* interpreter) { interpreter->PushStack(0); });

	// 0 through 9 pop a stack symbol, multiply it by ten, add themselves to the multiplied number and push the result to the stack.
	for (SymbolType i = '0'; i <= '9'; i++)
	{
		SymbolMap[i] = std::make_shared<NativeDefinition>([i](Emmental* interpreter)
		{
			SymbolType popped = interpreter->PopStack();
			interpreter->PushStack(i + popped * 10);
		});
	}

	// Add two stack symbols and push result to stack
	SymbolMap['+'] = std::make_shared<NativeDefinition>([](Emmental* interpreter) { interpreter->PushStack(interpreter->PopStack() + interpreter->PopStack()); });
	// Subtract first from second stack symbol and push result to stack
	SymbolMap['-'] = std::make_shared<NativeDefinition>([](Emmental* interpreter) 
	{ 
		SymbolType first = interpreter->PopStack();
		SymbolType second = interpreter->PopStack();

		interpreter->PushStack(second - first); 
	});
	// Push discrete log2 (highest set bit) of stack symbol (0 is treated as 256)
	SymbolMap['~'] = std::make_shared<NativeDefinition>([](Emmental* interpreter) 
	{ 
		SymbolType symbol = interpreter->PopStack();
		
		if (symbol == 0)
			symbol = 256;

		SymbolType log2 = (SymbolType)std::log2(symbol);
		interpreter->PushStack(log2); 
	});
	// Enqueue top stack symbol (doesn't remove it from the stack)
	SymbolMap['^'] = std::make_shared<NativeDefinition>([](Emmental* interpreter)
	{
		SymbolType symbol = interpreter->PopStack();
		interpreter->Enqueue(symbol);
		interpreter->PushStack(symbol);
	});
	// Dequeue to stack
	SymbolMap['^'] = std::make_shared<NativeDefinition>([](Emmental* interpreter)
	{
		SymbolType symbol = interpreter->Dequeue();
		interpreter->PushStack(symbol);
	});
	// Duplicate front queue symbol
	SymbolMap[':'] = std::make_shared<NativeDefinition>([](Emmental* interpreter)
	{
		SymbolType symbol = interpreter->Dequeue();
		interpreter->PushStack(symbol);
		interpreter->PushStack(symbol);
	});
	// Pop stack to output
	SymbolMap['.'] = std::make_shared<NativeDefinition>([](Emmental* interpreter)
	{
		SymbolType symbol = interpreter->PopStack();
		interpreter->OutputStream->put(symbol);
	});
	// Get input symbol and push to stack
	SymbolMap[','] = std::make_shared<NativeDefinition>([](Emmental* interpreter)
	{
		SymbolType symbol;
		interpreter->InputStream->get(symbol);
		interpreter->PushStack(symbol);
	});
	// For convenience, ';' puts ';' on the stack.
	SymbolMap[';'] = std::make_shared<NativeDefinition>([](Emmental* interpreter) { interpreter->PushStack(';'); });
	// Eval: Interpret the top stack symbol
	SymbolMap['?'] = std::make_shared<NativeDefinition>([](Emmental* interpreter)
	{
		SymbolType symbol = interpreter->PopStack();
		interpreter->Interpret(symbol);
	});
	
	// This is the main command of the Emmental: Supplant.
	// Pop a symbol and a program from the stack. Redefine the symbol as the popped program.
	SymbolMap['!'] = std::make_shared<NativeDefinition>([](Emmental* interpreter)
	{
		SymbolType symbol = interpreter->PopStack();
		std::vector<SymbolType> program = interpreter->PopProgram();

		interpreter->Redefine(
			symbol, 
			std::make_shared<InterpretedDefinition>(program, interpreter->CopyDefinitions())
		);

	});
}

EmmentalDefinition* Emmental::GetDefinition(SymbolType symbol, const std::map<SymbolType, std::shared_ptr<EmmentalDefinition>>& state)
{
	auto result = state.find(symbol);

	if (result == state.end())
		return nullptr;

	return result->second.get();
}