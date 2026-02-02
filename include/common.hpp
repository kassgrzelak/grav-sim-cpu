//
// Created by kassie on 01/02/2026.
//

#ifndef GRAV_SIM_CPU_COMMON_HPP
#define GRAV_SIM_CPU_COMMON_HPP

#include <vector>
#include <glm/vec2.hpp>

using BodyIndex_t = uint32_t;

using Vec2It_t = std::vector<glm::vec2>::iterator;
using FloatIt_t = std::vector<float>::iterator;
using IndexIt_t = std::vector<BodyIndex_t>::iterator;

#endif //GRAV_SIM_CPU_COMMON_HPP