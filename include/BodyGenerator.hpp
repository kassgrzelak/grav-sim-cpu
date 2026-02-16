//
// Created by kassie on 31/01/2026.
//

#ifndef GRAV_SIM_CPU_BODY_GENERATOR_HPP
#define GRAV_SIM_CPU_BODY_GENERATOR_HPP

#include "config.hpp"

#include <vector>

// constexpr float GALAXY_OUTER_RADIUS = 500.0f;
// constexpr float GALAXY_INNER_RADIUS = 200.0f;
// constexpr float GALAXY_PACK_DISTANCE = 8.4f;
// constexpr float GALAXY_CENTER_MASS = 1e7f;
// constexpr float GALAXY_CENTER_DIAMETER = 20.0f;
// constexpr float GALAXY_OTHER_MASS = 10.0f;
//
// enum class GenerationType
// {
// 	Galaxy, // Spawn bodies evenly in a ring around a central mass.
// };
//
// constexpr auto BODY_GENERATION = GenerationType::Galaxy;
// constexpr float GALAXY_OTHER_DIAMETER = 2.0f;

// TODO: config file, read at startup to spawn bodies according to config file

class BodyGenerator
{
public:
	static void generateBodies(std::vector<glm::vec2>& positions, std::vector<glm::vec2>& velocities,
		std::vector<float>& masses, std::vector<float>& diameters);

private:
	struct GalaxyParams
	{
		glm::vec2 position;
		glm::vec2 velocity;
		float outerRadius;
		float innerRadius;
		float packDistance;
		float centerMass;
		float centerDiameter;
		float outerMass;
		float outerDiameter;
		bool oppositeSpin;
	};

	static void generateGalaxyBodies(const GalaxyParams& params, std::vector<glm::vec2>& positions,
		std::vector<glm::vec2>& velocities, std::vector<float>& masses, std::vector<float>& diameters);
};

#endif //GRAV_SIM_CPU_BODY_GENERATOR_HPP