#pragma once

class EmmentalDefinition
{
public:
	virtual void Execute(class Emmental* interpreter, std::size_t recursionLevel) = 0;
};

