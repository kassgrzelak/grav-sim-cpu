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

void BodyGenerator::generateBodies(const char* generationPath, std::vector<glm::vec2>& positions,
	std::vector<glm::vec2>& velocities, std::vector<float>& masses, std::vector<float>& diameters)
{
	std::ifstream file(generationPath);
	if (!file.is_open())
		throw std::runtime_error(std::format("Failed to open generation file given path {}.", generationPath));

	int lineNum = 0;
	std::string line;
	while (std::getline(file, line))
	{
		++lineNum;

		// Skip empty lines and comments.
		if (line.empty() || line[0] == '#')
			continue;

		std::stringstream ss(line);

		std::string generationType;
		ss >> generationType;

#define PARSE_POSITION() \
	PARSE_FLOAT(positionX); \
	PARSE_FLOAT(positionY); \
	glm::vec2 position = {positionX, positionY}

#define PARSE_VELOCITY() \
	PARSE_FLOAT(velocityX); \
	PARSE_FLOAT(velocityY); \
	glm::vec2 velocity = {velocityX, velocityY}

#define PARSE_FLOAT(varName) \
	float varName; \
	ss >> varName

		// TODO: Grid generation.
		if (generationType == "SINGLE")
		{
			PARSE_POSITION();
			PARSE_VELOCITY();

			PARSE_FLOAT(mass);
			PARSE_FLOAT(diameter);

			SingleParams params{position, velocity, mass, diameter};

			generateSingleBody(params, positions, velocities, masses, diameters);
		}
		else if (generationType == "RECTPACK")
		{
			PARSE_POSITION();
			PARSE_VELOCITY();

			PARSE_FLOAT(width);
			PARSE_FLOAT(height);
			PARSE_FLOAT(packDistance);
			PARSE_FLOAT(bodyMass);
			PARSE_FLOAT(bodyDiameter);

			RectPackParams params{position, velocity, width, height, packDistance, bodyMass, bodyDiameter};

			generateRectPackBodies(params, positions, velocities, masses, diameters);
		}
		else if (generationType == "CIRCLEPACK")
		{
			PARSE_POSITION();
			PARSE_VELOCITY();

			PARSE_FLOAT(radius);
			PARSE_FLOAT(packDistance);
			PARSE_FLOAT(bodyMass);
			PARSE_FLOAT(bodyDiameter);

			CirclePackParams params{position, velocity, radius, packDistance, bodyMass, bodyDiameter};

			generateCirclePackBodies(params, positions, velocities, masses, diameters);
		}
		else if (generationType == "GALAXY")
		{
			PARSE_POSITION();
			PARSE_VELOCITY();

			PARSE_FLOAT(outerRadius);
			PARSE_FLOAT(innerRadius);
			PARSE_FLOAT(packDistance);
			PARSE_FLOAT(centerMass);
			PARSE_FLOAT(centerDiameter);
			PARSE_FLOAT(outerMass);
			PARSE_FLOAT(outerDiameter);

			bool oppositeSpin;
			ss >> oppositeSpin;

			GalaxyParams params{position, velocity, outerRadius, innerRadius, packDistance, centerMass, centerDiameter,
				outerMass, outerDiameter, oppositeSpin};

			generateGalaxyBodies(params, positions, velocities, masses, diameters);
		}
		else
			throw std::runtime_error(std::format("Unknown generation type '{}' on line {}.", generationType, lineNum));
#undef PARSE_POSITION
#undef PARSE_VELOCITY
#undef PARSE_FLOAT

		if (ss.fail())
			throw std::runtime_error(std::format("Failed reading parameters on line {}. Check you have "
				"the right type/number of parameters.", lineNum));
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
			const float orbitalSpeed = (params.counterClockwise ? 1.0f : -1.0f) * sqrtf(g_gravConst * params.centerMass / dist);

			positions.emplace_back(x, y);
			velocities.emplace_back(tangent * orbitalSpeed + params.velocity);
			masses.push_back(params.outerMass);
			diameters.push_back(params.outerDiameter);
		}
	}
}

void BodyGenerator::generateRectPackBodies(const RectPackParams& params, std::vector<glm::vec2>& positions,
	std::vector<glm::vec2>& velocities, std::vector<float>& masses, std::vector<float>& diameters)
{
	const float halfPackDistance = params.packDistance / 2.0f;
	const float halfWidth = params.width / 2;
	const float halfHeight = params.height / 2;
	const float h = sqrtf(3.0f) * halfPackDistance;
	float offset = 0.0f;

	for (float y = params.position.y - halfHeight; y < params.position.y + halfHeight; y += h)
	{
		if (offset == 0.0f)
			offset = halfPackDistance;
		else
			offset = 0.0f;

		for (float x = params.position.x - halfWidth + offset; x < params.position.x + halfWidth;
			x += params.packDistance)
		{
			positions.emplace_back(x, y);
			velocities.emplace_back(params.velocity);
			masses.push_back(params.bodyMass);
			diameters.push_back(params.bodyDiameter);
		}
	}
}

void BodyGenerator::generateCirclePackBodies(const CirclePackParams& params, std::vector<glm::vec2>& positions,
                                             std::vector<glm::vec2>& velocities, std::vector<float>& masses, std::vector<float>& diameters)
{
	const float halfPackDistance = params.packDistance / 2.0f;
	const float radiusSquared = params.radius * params.radius;
	const float h = sqrtf(3.0f) * halfPackDistance;
	float offset = 0.0f;

	for (float y = params.position.y - params.radius; y < params.position.y + params.radius; y += h)
	{
		if (offset == 0.0f)
			offset = halfPackDistance;
		else
			offset = 0.0f;

		for (float x = params.position.x - params.radius + offset; x < params.position.x + params.radius;
			x += params.packDistance)
		{
			const float sqrDist = glm::distance2(params.position, glm::vec2{x, y});
			if (sqrDist > radiusSquared)
				continue;

			positions.emplace_back(x, y);
			velocities.emplace_back(params.velocity);
			masses.push_back(params.bodyMass);
			diameters.push_back(params.bodyDiameter);
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
