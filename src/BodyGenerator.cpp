//
// Created by kassie on 31/01/2026.
//

#include "BodyGenerator.hpp"

#include <cmath>

void BodyGenerator::generateBodies(std::vector<glm::vec2>& positions, std::vector<glm::vec2>& velocities,
		std::vector<float>& masses, std::vector<float>& diameters)
{
	switch (BODY_GENERATION)
	{
		case GenerationType::Galaxy: return generateGalaxyBodies(positions, velocities, masses, diameters);
	}
}

void BodyGenerator::generateGalaxyBodies(std::vector<glm::vec2>& positions, std::vector<glm::vec2>& velocities,
		std::vector<float>& masses, std::vector<float>& diameters)
{
	positions.push_back(SCREEN_CENTER);
	velocities.emplace_back();
	masses.push_back(GALAXY_CENTER_MASS);
	diameters.push_back(GALAXY_CENTER_DIAMETER);

	for (float dist = GALAXY_INNER_RADIUS; dist < GALAXY_OUTER_RADIUS;
		dist += (GALAXY_OUTER_RADIUS - GALAXY_INNER_RADIUS) / GALAXY_RINGS)
		for (float angle = dist * 10.1873482f; angle < 2.0 * PI + dist * 10.1873482f; angle += 1.0f / (dist * GALAXY_DENSITY))
		{
			positions.push_back(SCREEN_CENTER + glm::vec2(dist * std::cos(angle), dist * std::sin(angle)));
			velocities.emplace_back(
				-sqrt(GRAV_CONST * static_cast<double>(GALAXY_CENTER_MASS) / dist) * std::sin(angle),
				sqrt(GRAV_CONST * static_cast<double>(GALAXY_CENTER_MASS) / dist) * std::cos(angle));
			masses.push_back(GALAXY_OTHER_MASS);
			diameters.push_back(GALAXY_OTHER_DIAMETER);
		}
}
