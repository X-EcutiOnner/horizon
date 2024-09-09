#include "Server/Char/Char.hpp"
#include "Core/Logging/Logger.hpp"
#include <random>

/**
 * Main entry point of the char-server application.
 * @param argc
 * @param argv
 * @return integer of shutdown signal.
 */
int main(int argc, const char * argv[])
{
	std::srand(std::time(nullptr));
	
	/* Parse Command Line Arguments */
	if (argc > 1)
		sChar->parse_exec_args(argv, argc);

	/* Read Char Configuration */
	if (!sChar->read_config())
		exit(1); // Stop process if the file can't be read.

	/* Initialize the Common Core */
	sChar->initialize();

	return 0;
}
