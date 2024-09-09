#include "Core/Logging/Logger.hpp"
#include "Server/Auth/Auth.hpp"
#include <random>
/**
 * Main Runtime Method
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char * argv[])
{
	std::srand(std::time(nullptr));
	
	if (argc > 1)
		sAuth->parse_exec_args(argv, argc);

	/*
	 * Read Configuration Settings for
	 * the Horizon Server.
	 */
	if (!sAuth->read_config())
		exit(1); // Stop process if the file can't be read.

	/**
	 * Initialize the Common Core
	 */
	sAuth->initialize();
	
	return 0;
}
