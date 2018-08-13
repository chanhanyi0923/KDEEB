#include "RenderException.h"


RenderException::RenderException()
{
}


RenderException::RenderException(const std::string & message):
	message(message)
{
}


RenderException::~RenderException()
{
}


const char * RenderException::what() const noexcept
{
	return this->message.c_str();
}

