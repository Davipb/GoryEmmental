#include "Emmental.h"
#include "NativeDefinition.h"

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
	// Push NULL to the stack
	SymbolMap['#'] = new NativeDefinition([](Emmental* interpreter) { interpreter->PushStack(0); });

	// 0 through 9 pop a stack symbol, multiply it by ten, add themselves to the multiplied number and push the result to the stack.
	for (SymbolType i = '0'; i <= '9'; i++)
	{
		SymbolMap[i] = new NativeDefinition([i](Emmental* interpreter)
		{
			SymbolType popped = interpreter->PopStack();
			interpreter->PushStack(i + popped * 10);
		});
	}

	// Add two stack symbols and push result to stack
	SymbolMap['+'] = new NativeDefinition([](Emmental* interpreter) { interpreter->PushStack(interpreter->PopStack() + interpreter->PopStack()); });
	// Subtract first from second stack symbol and push result to stack
	SymbolMap['-'] = new NativeDefinition([](Emmental* interpreter) 
	{ 
		SymbolType first = interpreter->PopStack();
		SymbolType second = interpreter->PopStack();

		interpreter->PushStack(second - first); 
	});
	// Push discrete log2 (highest set bit) of stack symbol (0 is treated as 256)
	SymbolMap['~'] = new NativeDefinition([](Emmental* interpreter) 
	{ 
		SymbolType symbol = interpreter->PopStack();
		
		if (symbol == 0)
			symbol = 256;

		SymbolType log2 = (SymbolType)std::log2(symbol);
		interpreter->PushStack(log2); 
	});
	// Enqueue top stack symbol (doesn't remove it from the stack)
	SymbolMap['^'] = new NativeDefinition([](Emmental* interpreter)
	{
		SymbolType symbol = interpreter->PopStack();
		interpreter->Enqueue(symbol);
		interpreter->PushStack(symbol);
	});
	// Dequeue to stack
	SymbolMap['^'] = new NativeDefinition([](Emmental* interpreter)
	{
		SymbolType symbol = interpreter->Dequeue();
		interpreter->PushStack(symbol);
	});
	// Duplicate front queue symbol
	SymbolMap[':'] = new NativeDefinition([](Emmental* interpreter)
	{
		SymbolType symbol = interpreter->Dequeue();
		interpreter->PushStack(symbol);
		interpreter->PushStack(symbol);
	});
	// Pop stack to output
	SymbolMap['.'] = new NativeDefinition([](Emmental* interpreter)
	{
		SymbolType symbol = interpreter->PopStack();
		interpreter->OutputStream->put(symbol);
	});
	// Get input symbol and push to stack
	SymbolMap[','] = new NativeDefinition([](Emmental* interpreter)
	{
		SymbolType symbol;
		interpreter->InputStream->get(symbol);
		interpreter->PushStack(symbol);
	});
	// For convenience, ';' puts ';' on the stack.
	SymbolMap[';'] = new NativeDefinition([](Emmental* interpreter) { interpreter->PushStack(';'); });

	// TODO: '!', '?'
}
