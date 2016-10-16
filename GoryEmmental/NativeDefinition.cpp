#include "NativeDefinition.h"

NativeDefinition::NativeDefinition(std::function<void(Emmental*, std::size_t)> function)
{
	Function = function;
}

void NativeDefinition::Execute(Emmental* interpreter, std::size_t recursionLevel)
{
	Function(interpreter, recursionLevel);
}
