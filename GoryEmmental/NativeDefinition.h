#pragma once
#include <functional>
#include "EmmentalDefinition.h"


class NativeDefinition :
	public EmmentalDefinition
{
public:	
	NativeDefinition(std::function<void(Emmental*, std::size_t)> function);
	virtual void Execute(Emmental* interpreter, std::size_t recursionLevel) override;

private:
	std::function<void(Emmental*, std::size_t)> Function;
};