//
// Created by kassie on 31/01/2026.
//

#ifndef GRAV_SIM_CPU_BODY_GENERATOR_HPP
#define GRAV_SIM_CPU_BODY_GENERATOR_HPP

#include "config.hpp"

#include <vector>

#include "Body.hpp"

class BodyGenerator
{
public:
	static void generateBodies(std::vector<Body>& bodies);

private:
	static void generateGalaxyBodies(std::vector<Body>& bodies);
};

#endif //GRAV_SIM_CPU_BODY_GENERATOR_HPP