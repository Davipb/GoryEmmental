#pragma once
#include <exception>
class EmmentalException : public std::exception
{
public:
	EmmentalException(const char* const message) : Message(message) {}
	virtual const char* what() const override { return Message; }

private:
	const char* const Message;
};

