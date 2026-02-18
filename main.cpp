#include <cstring>
#include <iostream>

#include "Sim.hpp"

int main(const int argc, const char* argv[])
{
	if (argc > 1 && strcmp(argv[1], "--help") == 0)
	{
		std::cout << "Options:\n" <<
			"\t-g | --generation: Path to generation config file. Looks for a file called generation.cfg in the same "
			"directory by default.\n" <<
			"\t--help: Display this message.\n" <<
			"\t-s | --simulation: Path to simulation config file. Looks for a file called simulation.cfg in the same "
		    "directory by default.";

		return 0;
	}

	// Config file default paths.
	const char* generationPath = "generation.cfg";
	const char* simulationPath = "simulation.cfg";

	for (int i = 1; i < argc; ++i)
	{
		const char* option = argv[i];

		if (strcmp(option, "-g") == 0 || strcmp(option, "--generation") == 0)
		{
			if (++i >= argc)
			{
				std::cerr << "No argument supplied for generation file.\n";
				return 64;
			}

			generationPath = argv[i];
		}
		else if (strcmp(option, "-s") == 0 || strcmp(option, "--simulation") == 0)
		{
			if (++i >= argc)
			{
				std::cerr << "No argument supplied for simulation file.\n";
				return 64;
			}

			simulationPath = argv[i];
		}
		else
		{
			std::cerr << "Unknown option '" << option << "'\n";
			return 64;
		}
	}

	try
	{
		loadSimulationFile(simulationPath);
		Sim sim(generationPath);
		sim.run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what();
		return -1;
	}

	return 0;
}
