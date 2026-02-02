//
// Created by kassie on 31/01/2026.
//

#ifndef GRAV_SIM_CPU_BODY_GENERATOR_HPP
#define GRAV_SIM_CPU_BODY_GENERATOR_HPP

#include "config.hpp"

#include <vector>

class BodyGenerator
{
public:
	static void generateBodies(std::vector<glm::vec2>& positions, std::vector<glm::vec2>& velocities,
		std::vector<float>& masses, std::vector<float>& diameters);

private:
	static void generateGalaxyBodies(std::vector<glm::vec2>& positions, std::vector<glm::vec2>& velocities,
		std::vector<float>& masses, std::vector<float>& diameters);
};

#endif //GRAV_SIM_CPU_BODY_GENERATOR_HPP