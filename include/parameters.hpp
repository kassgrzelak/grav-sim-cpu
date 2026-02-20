//
// Created by kassie on 30/01/2026.
//

#ifndef GRAV_SIM_CPU_CONFIG_HPP
#define GRAV_SIM_CPU_CONFIG_HPP

#include <glm/glm.hpp>

#include "colormap.hpp"

constexpr int MAX_COLORMAP_MODE = 2;

enum class ColormapMode
{
    None, Speed, Velocity
};

const char* colormapModeToString(ColormapMode mode);

// Defined in parameters.cpp when loading simulation config file.
extern float g_theta;
extern float g_gravConst;
extern float g_gravSmoothness;

extern glm::vec2 g_screenDims;
extern bool g_resizable;

extern int g_targetFPS;
extern float g_timeScale;
extern float g_deltaTime;

extern Color3 g_bodyColor;
extern int g_bodyAlpha;

extern ColormapMode g_colormapMode;
extern float g_colormapMaxSpeed;
extern float g_colormapMaxSqrSpeed;

void loadSimulationFile(const char* simulationPath);

#endif //GRAV_SIM_CPU_CONFIG_HPP