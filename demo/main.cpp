#include <iostream>

#include "IController.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* args[])
{
	try
	{
		const auto controller = createInstance();
		controller->init();
	} catch (std::runtime_error const& e)
	{
		std::cerr << "Unhandled exception: " << e.what() << '\n';
	}

	return 0;
}