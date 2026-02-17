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

	const char* generationPath = "generation.cfg";
	const char* simulationPath = "simulation.cfg";

	for (int i = 1; i < argc; ++i)
	{
		const char* arg = argv[i];

		if (strcmp(arg, "-g") == 0 || strcmp(arg, "--generation") == 0)
		{
			if (i < argc - 1)
				++i;
			else
			{
				std::cerr << "No argument supplied for generation file.\n";
				return 64;
			}

			generationPath = argv[i];
		}
		else if (strcmp(arg, "-s") == 0 || strcmp(arg, "--simulation") == 0)
		{
			if (i < argc - 1)
				++i;
			else
			{
				std::cerr << "No argument supplied for simulation file.\n";
				return 64;
			}

			simulationPath = argv[i];
		}
		else
		{
			std::cerr << "Unknown option '" << arg << "'\n";
			return 64;
		}
	}

	loadSimulationFile(simulationPath);
	Sim sim(generationPath);

	try
	{
		sim.run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what();
		return -1;
	}

	return 0;
}
