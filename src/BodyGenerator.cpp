//
// Created by kassie on 31/01/2026.
//

#include "BodyGenerator.hpp"

#include <cmath>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp>

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
	positions.push_back(g_screenCenter);
	velocities.emplace_back();
	masses.push_back(GALAXY_CENTER_MASS);
	diameters.push_back(GALAXY_CENTER_DIAMETER);

	static constexpr float HALF_PACK_DISTANCE = GALAXY_PACK_DISTANCE / 2.0f;
	static constexpr float OUTER_RADIUS_SQUARED = GALAXY_OUTER_RADIUS * GALAXY_OUTER_RADIUS;
	static constexpr float INNER_RADIUS_SQUARED = GALAXY_INNER_RADIUS * GALAXY_INNER_RADIUS;

	const float h = sqrtf(3.0f) * HALF_PACK_DISTANCE;
	float offset = 0.0f;

	for (float y = g_screenCenter.y - GALAXY_OUTER_RADIUS; y < g_screenCenter.y + GALAXY_OUTER_RADIUS; y += h)
	{
		if (offset == 0.0f)
			offset = HALF_PACK_DISTANCE;
		else
			offset = 0.0f;

		for (float x = g_screenCenter.x - GALAXY_OUTER_RADIUS + offset; x < g_screenCenter.x + GALAXY_OUTER_RADIUS;
			x += GALAXY_PACK_DISTANCE)
		{
			const float sqrDist = glm::distance2(g_screenCenter, glm::vec2{x, y});
			if (sqrDist > OUTER_RADIUS_SQUARED || sqrDist < INNER_RADIUS_SQUARED)
				continue;

			const float dist = sqrtf(sqrDist);
			const glm::vec2 rel = glm::vec2{x, y} - g_screenCenter;
			const glm::vec2 tangent = glm::normalize(glm::vec2{-rel.y, rel.x});
			const float orbitalSpeed = sqrtf(GRAV_CONST * GALAXY_CENTER_MASS / dist);

			positions.emplace_back(x, y);
			velocities.emplace_back(tangent * orbitalSpeed);
			masses.push_back(GALAXY_OTHER_MASS);
			diameters.push_back(GALAXY_OTHER_DIAMETER);
		}
	}
}
