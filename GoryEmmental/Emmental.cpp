#include "Emmental.h"
#include "NativeDefinition.h"
#include "InterpretedDefinition.h"
#include "Util.h"
#include "Globals.h"
#include "EmmentalException.h"

static const Util::ConsoleColor ErrorColor = Util::ConsoleColor::BrightRed;
static const Util::ConsoleColor WarningColor = Util::ConsoleColor::BrightYellow;

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
		if (!Globals::QuietMode)
		{
			Util::Colorize(ErrorColor, ErrorStream);
			ErrorStream << "Error: ";
			Util::Colorize(Util::ConsoleColor::Default, ErrorStream);

			ErrorStream << "Tried popping symbol from empty stack.";

			if (Globals::LenientMode)
				ErrorStream << " Returning default value.";

			ErrorStream << std::endl;
		}

		if (!Globals::LenientMode)
			throw EmmentalException("Tried popping symbol from empty stack.");

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
		if (!Globals::QuietMode)
		{
			Util::Colorize(ErrorColor, ErrorStream);
			ErrorStream << "Error: ";
			Util::Colorize(Util::ConsoleColor::Default, ErrorStream);
			ErrorStream << "Tried popping program from empty stack.";

				if (Globals::LenientMode)
					ErrorStream << " Returning default program.";

			ErrorStream << std::endl;
		}

		if (!Globals::LenientMode)
			throw EmmentalException("Tried popping program from empty stack.");

		return ProgramT();
	}

	ProgramT result;
	SymbolT symbol = PopSymbol();
	
	while (symbol != ';')
	{
		result.push_back(symbol);

		if (ProgramStack.empty())
		{
			if (!Globals::QuietMode)
			{
				Util::Colorize(ErrorColor, ErrorStream);
				ErrorStream << "Error: ";
				Util::Colorize(Util::ConsoleColor::Default, ErrorStream);
				ErrorStream << "Stack ran out before ";
				Util::DescribeSymbol(';', ErrorStream);
				ErrorStream << " was found to terminate a program.";

				if (Globals::LenientMode)
					ErrorStream << " Returning incomplete program.";

				ErrorStream << std::endl;
			}

			if (!Globals::LenientMode)
				throw EmmentalException("Stack ran out before ';' was found to terminate a program.");

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
		if (!Globals::QuietMode)
		{
			Util::Colorize(ErrorColor, ErrorStream);
			ErrorStream << "Error: ";
			Util::Colorize(Util::ConsoleColor::Default, ErrorStream);
			ErrorStream << "Tried to push symbol ";
			Util::DescribeSymbol(item, ErrorStream);
			ErrorStream << " to full stack.";

			if (Globals::LenientMode)
				ErrorStream << " Ignoring.";

			ErrorStream << std::endl;
		}

		if (!Globals::LenientMode)
			throw EmmentalException("Stack full.");

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
		if (!Globals::QuietMode)
		{
			Util::Colorize(ErrorColor, ErrorStream);
			ErrorStream << "Error: ";
			Util::Colorize(Util::ConsoleColor::Default, ErrorStream);

			ErrorStream << "Tried dequeuing symbol from empty queue.";

			if (Globals::LenientMode)
				ErrorStream << " Returning default value.";

			ErrorStream << std::endl;
		}

		if (!Globals::LenientMode)
			throw EmmentalException("Tried dequeuing symbol from empty queue.");

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
		if (!Globals::QuietMode)
		{
			Util::Colorize(ErrorColor, ErrorStream);
			ErrorStream << "Error: ";
			Util::Colorize(Util::ConsoleColor::Default, ErrorStream);
			ErrorStream << "Tried to enqueue symbol ";
			Util::DescribeSymbol(item, ErrorStream);
			ErrorStream << " to full queue.";

			if (Globals::LenientMode)
				ErrorStream << " Ignoring.";

			ErrorStream << std::endl;
		}

		if (!Globals::LenientMode)
			throw EmmentalException("Queue full.");

		return;
	}

	ProgramQueue.push(item);
}

void Emmental::ClearQueue()
{
	while (!ProgramQueue.empty())
		ProgramQueue.pop();
}

std::shared_ptr<EmmentalDefinition> Emmental::GetDefinition(SymbolT symbol) const { return GetDefinition(symbol, SymbolMap); }

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
		if (!Globals::QuietMode)
		{
			Util::Colorize(ErrorColor, ErrorStream);
			ErrorStream << "Error: ";
			Util::Colorize(Util::ConsoleColor::Default, ErrorStream);
			ErrorStream << "Recursion level too high.";

			if (Globals::LenientMode)
			{
				ErrorStream << " Ignoring symbol ";
				Util::DescribeSymbol(symbol, ErrorStream);
				ErrorStream << "." << std::endl;
			}

			ErrorStream << std::endl;
		}

		if (!Globals::LenientMode)
			throw EmmentalException("Recursion level too high.");

		return;
	}

	std::shared_ptr<EmmentalDefinition> definition = GetDefinition(symbol, state);

	if (definition)
	{
		definition->Execute(this, recursionLevel + 1);
	}
	else
	{
		if (!Globals::QuietMode)
		{
			Util::Colorize(WarningColor, ErrorStream);
			ErrorStream << "Warning: ";
			Util::Colorize(Util::ConsoleColor::Default, ErrorStream);
			ErrorStream << "Tried to interpret undefined symbol ";
			Util::DescribeSymbol(symbol, ErrorStream);
			ErrorStream << ". Ignoring symbol." << std::endl;
		}
	}
}

void Emmental::Redefine(SymbolT symbol, std::shared_ptr<EmmentalDefinition> definition)
{
	if (definition)
		SymbolMap[symbol] = definition;
	else
		Undefine(symbol);
}

void Emmental::Undefine(SymbolT symbol)
{
	SymbolMap.erase(symbol);
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
	SymbolMap['v'] = std::make_shared<NativeDefinition>([](Emmental* interpreter, std::size_t)
	{
		SymbolT symbol = interpreter->Dequeue();
		interpreter->Push(symbol);
	});
	// Duplicate top stack symbol
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

		if (program.empty() && Globals::OptimizeProgram)
		{
			// If the program is empty, just undefine the symbol (make it a no-op)
			interpreter->Undefine(symbol);
		}
		else if (program.size() == 1 && Globals::OptimizeProgram)
		{
			// For single-symbol programs, just set the definition to the single symbol's definition
			std::shared_ptr<EmmentalDefinition> definition = interpreter->GetDefinition(program[0]);

			// Redefine() will take care of undefining the symbol if 'definition' is nullptr (aka single symbol in program is undefined/no-op)
			interpreter->Redefine(symbol, definition);
		}
		else
		{
			SymbolMapT state = interpreter->CopyDefinitions(program);

			interpreter->Redefine(
				symbol,
				std::make_shared<InterpretedDefinition>(program, state)
			);
		}

	});
}

std::shared_ptr<EmmentalDefinition> Emmental::GetDefinition(SymbolT symbol, const SymbolMapT& state) const
{
	auto result = state.find(symbol);

	if (result == state.end())
		return nullptr;

	return result->second;
}