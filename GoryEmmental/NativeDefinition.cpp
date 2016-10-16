#include "NativeDefinition.h"

NativeDefinition::NativeDefinition(std::function<void(Emmental*)> function)
{
	Function = function;
}

void NativeDefinition::Execute(Emmental* interpreter)
{
	Function(interpreter);
}
