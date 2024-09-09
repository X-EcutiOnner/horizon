#include "Server/Zone/Zone.hpp"

#include <random>
#include <ctime>

/**
 * Zone Server Main runtime entrypoint.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char * argv[])
{
	std::srand(std::time(nullptr));
	
	if (argc > 1)
		sZone->parse_exec_args(argv, argc);

	/*
	 * Read Configuration Settings for
	 * the Zoneacter Server.
	 */
	if (!sZone->read_config())
		exit(1); // Stop process if the file can't be read.

	/**
	 * Initialize the Common Core
	 */
	sZone->initialize();
	return 0;
}
