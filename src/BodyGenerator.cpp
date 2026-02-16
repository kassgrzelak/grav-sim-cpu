//
// Created by kassie on 31/01/2026.
//

#include "BodyGenerator.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp>

void BodyGenerator::generateBodies(std::vector<glm::vec2>& positions, std::vector<glm::vec2>& velocities,
	std::vector<float>& masses, std::vector<float>& diameters)
{
	std::ifstream file("generation.cfg");
	if (!file.is_open())
		throw std::runtime_error("Failed to open generation config file.");

	int lineNum = 0;
	std::string line;
	while (std::getline(file, line))
	{
		++lineNum;

		if (line.empty() || line[0] == '#')
			continue;

		std::stringstream ss(line);

		std::string generationType;
		ss >> generationType;

		if (generationType == "GALAXY")
		{
			float positionX;
			float positionY;
			ss >> positionX >> positionY;
			glm::vec2 position = {positionX, positionY};

			float velocityX;
			float velocityY;
			ss >> velocityX >> velocityY;
			glm::vec2 velocity = {velocityX, velocityY};

			float outerRadius;
			ss >> outerRadius;

			float innerRadius;
			ss >> innerRadius;

			float packDistance;
			ss >> packDistance;

			float centerMass;
			ss >> centerMass;

			float centerDiameter;
			ss >> centerDiameter;

			float outerMass;
			ss >> outerMass;

			float outerDiameter;
			ss >> outerDiameter;

			bool oppositeSpin;
			ss >> oppositeSpin;

			if (ss.fail())
				throw std::runtime_error(std::format("Failed reading parameters on line {}. Check you have "
					"the right type/number of parameters.", lineNum));

			GalaxyParams params{position, velocity, outerRadius, innerRadius, packDistance, centerMass, centerDiameter,
				outerMass, outerDiameter, oppositeSpin};

			generateGalaxyBodies(params, positions, velocities, masses, diameters);
		}
		else if (generationType == "SINGLE")
		{
			float positionX;
			float positionY;
			ss >> positionX >> positionY;
			glm::vec2 position = {positionX, positionY};

			float velocityX;
			float velocityY;
			ss >> velocityX >> velocityY;
			glm::vec2 velocity = {velocityX, velocityY};

			float mass;
			ss >> mass;

			float diameter;
			ss >> diameter;

			SingleParams params{position, velocity, mass, diameter};

			generateSingleBody(params, positions, velocities, masses, diameters);
		}
		else
		{
			throw std::runtime_error(std::format("Unknown generation type '{}' on line {}", generationType, lineNum));
		}
	}
}

void BodyGenerator::generateGalaxyBodies(const GalaxyParams& params, std::vector<glm::vec2>& positions,
	std::vector<glm::vec2>& velocities,
	std::vector<float>& masses, std::vector<float>& diameters)
{
	positions.push_back(params.position);
	velocities.emplace_back(params.velocity);
	masses.push_back(params.centerMass);
	diameters.push_back(params.centerDiameter);

	const float halfPackDistance = params.packDistance / 2.0f;
	const float outerRadiusSquared = params.outerRadius * params.outerRadius;
	const float innerRadiusSquared = params.innerRadius * params.innerRadius;

	const float h = sqrtf(3.0f) * halfPackDistance;
	float offset = 0.0f;

	for (float y = params.position.y - params.outerRadius; y < params.position.y + params.outerRadius; y += h)
	{
		if (offset == 0.0f)
			offset = halfPackDistance;
		else
			offset = 0.0f;

		for (float x = params.position.x - params.outerRadius + offset; x < params.position.x + params.outerRadius;
			x += params.packDistance)
		{
			const float sqrDist = glm::distance2(params.position, glm::vec2{x, y});
			if (sqrDist > outerRadiusSquared || sqrDist < innerRadiusSquared)
				continue;

			const float dist = sqrtf(sqrDist);
			const glm::vec2 rel = glm::vec2{x, y} - params.position;
			const glm::vec2 tangent = glm::normalize(glm::vec2{-rel.y, rel.x});
			const float orbitalSpeed = (params.counterClockwise ? 1.0f : -1.0f) * sqrtf(GRAV_CONST * params.centerMass / dist);

			positions.emplace_back(x, y);
			velocities.emplace_back(tangent * orbitalSpeed + params.velocity);
			masses.push_back(params.outerMass);
			diameters.push_back(params.outerDiameter);
		}
	}
}

void BodyGenerator::generateSingleBody(const SingleParams& params, std::vector<glm::vec2>& positions,
	std::vector<glm::vec2>& velocities, std::vector<float>& masses, std::vector<float>& diameters)
{
	positions.push_back(params.position);
	velocities.push_back(params.velocity);
	masses.push_back(params.mass);
	diameters.push_back(params.diameter);
}
