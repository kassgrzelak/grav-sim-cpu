//
// Created by kassie on 11/02/2026.
//

#ifndef GRAV_SIM_CPU_BODY_HPP
#define GRAV_SIM_CPU_BODY_HPP
#include <glm/vec2.hpp>

struct Body
{
	glm::vec2 position;
	glm::vec2 velocity;
	float mass;
	float diameter;
};

#endif //GRAV_SIM_CPU_BODY_HPP