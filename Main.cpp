#include <exception>
#include <iostream>

#include "Application.h"

int main(int argc, char *argv[])
{
	Application * app = new Application;
	try {
		app->Run(argv[1], argv[2]);
		delete app;
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
