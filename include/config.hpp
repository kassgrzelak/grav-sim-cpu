//
// Created by kassie on 30/01/2026.
//

#ifndef GRAV_SIM_CPU_CONFIG_HPP
#define GRAV_SIM_CPU_CONFIG_HPP

#include <glm/glm.hpp>
#include <raylib.h>

// TODO: config file to determine these constants? hopefully won't affect performance too much, testing would be needed.

constexpr float THETA = 0.5f;
constexpr float GRAV_CONST = 1.0f;
constexpr float GRAV_SMOOTHNESS = 1.0f;

inline glm::vec2 g_screenDims = {1600, 900};
inline glm::vec2 g_screenCenter = g_screenDims / 2.0f;

constexpr int TARGET_FPS = 60;
constexpr float TIME_SCALE = 1.0f;
constexpr float DELTA_TIME = TIME_SCALE / TARGET_FPS;

constexpr Color BODY_COLOR = WHITE;

#endif //GRAV_SIM_CPU_CONFIG_HPP