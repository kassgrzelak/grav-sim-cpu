//
// Created by kassie on 31/01/2026.
//

#ifndef GRAV_SIM_CPU_BODY_GENERATOR_HPP
#define GRAV_SIM_CPU_BODY_GENERATOR_HPP

#include "parameters.hpp"

#include <vector>

class BodyGenerator
{
public:
	static void generateBodies(const char* generationPath, std::vector<glm::vec2>& positions,
		std::vector<glm::vec2>& velocities, std::vector<float>& masses, std::vector<float>& diameters);

private:
	struct SingleParams
	{
		glm::vec2 position;
		glm::vec2 velocity;
		float mass;
		float diameter;
	};

	static void generateSingleBody(const SingleParams& params, std::vector<glm::vec2>& positions,
		std::vector<glm::vec2>& velocities, std::vector<float>& masses, std::vector<float>& diameters);

	struct RectPackParams
	{
		glm::vec2 position;
		glm::vec2 velocity;
		float width;
		float height;
		float packDistance;
		float bodyMass;
		float bodyDiameter;
	};

	static void generateRectPackBodies(const RectPackParams& params, std::vector<glm::vec2>& positions,
		std::vector<glm::vec2>& velocities, std::vector<float>& masses, std::vector<float>& diameters);

	struct CirclePackParams
	{
		glm::vec2 position;
		glm::vec2 velocity;
		float radius;
		float packDistance;
		float bodyMass;
		float bodyDiameter;
	};

	static void generateCirclePackBodies(const CirclePackParams& params, std::vector<glm::vec2>& positions,
		std::vector<glm::vec2>& velocities, std::vector<float>& masses, std::vector<float>& diameters);

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
		bool counterClockwise;
	};

	static void generateGalaxyBodies(const GalaxyParams& params, std::vector<glm::vec2>& positions,
		std::vector<glm::vec2>& velocities, std::vector<float>& masses, std::vector<float>& diameters);

};

#endif //GRAV_SIM_CPU_BODY_GENERATOR_HPP