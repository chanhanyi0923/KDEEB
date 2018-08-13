#pragma once

#include <stdexcept>
#include <string>

class RenderException : public std::exception
{
	std::string message;
public:
	RenderException();
	RenderException(const std::string & message);
	~RenderException();
	const char *what() const noexcept;
};

