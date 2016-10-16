#pragma once
#include <functional>
#include "EmmentalDefinition.h"


class NativeDefinition :
	public EmmentalDefinition
{
public:	
	NativeDefinition(std::function<void(Emmental*)> function);
	virtual void Execute(Emmental* interpreter) override;

private:
	std::function<void(Emmental*)> Function;
};