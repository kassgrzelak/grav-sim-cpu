//
// Created by kassie on 30/01/2026.
//

#ifndef GRAV_SIM_CPU_CONFIG_HPP
#define GRAV_SIM_CPU_CONFIG_HPP

#include <glm/glm.hpp>
#include <raylib.h>

constexpr float THETA = 0.5f;
constexpr float GRAV_CONST = 1.0f;
constexpr float GRAV_SMOOTHNESS = 1.0f;

inline glm::vec2 g_screenDims = {1600, 900};
inline glm::vec2 g_screenCenter = g_screenDims / 2.0f;

constexpr int TARGET_FPS = 60;
constexpr float TIME_SCALE = 1.0f;
constexpr float DELTA_TIME = TIME_SCALE / TARGET_FPS;

constexpr float GALAXY_OUTER_RADIUS = 500.0f;
constexpr float GALAXY_INNER_RADIUS = 200.0f;
constexpr float GALAXY_PACK_DISTANCE = 4.4f;
constexpr float GALAXY_CENTER_MASS = 1e7f;
constexpr float GALAXY_CENTER_DIAMETER = 20.0f;
constexpr float GALAXY_OTHER_MASS = 10.0f;
constexpr float GALAXY_OTHER_DIAMETER = 2.0f;

enum class GenerationType
{
	Galaxy, // Spawn bodies in rings around a central mass.
};

constexpr auto BODY_GENERATION = GenerationType::Galaxy;

constexpr Color BODY_COLOR = WHITE;

#endif //GRAV_SIM_CPU_CONFIG_HPP