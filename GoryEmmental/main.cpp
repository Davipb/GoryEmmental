#include <iostream>
#include <string>
#include "Emmental.h"

int main()
{
	Emmental interpreter(std::cin, std::cout);

	while (true)
	{
		std::cout << "> ";
		SymbolType symbol;
		std::cin >> symbol;

		interpreter.Interpret(symbol);

		std::cout << std::endl;

		std::cout << "Stack: ";
		std::stack<SymbolType> stack = interpreter.GetStack();
		while (!stack.empty())
		{
			std::cout << " " << std::to_string(stack.top());
			stack.pop();
		}
		std::cout << std::endl;

		std::cout << "Queue: ";
		std::queue<SymbolType> queue = interpreter.GetQueue();
		while (!queue.empty())
		{
			std::cout << " " << std::to_string(queue.front());
			queue.pop();
		}
		std::cout << std::endl;
	}
}