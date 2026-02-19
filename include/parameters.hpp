//
// Created by kassie on 30/01/2026.
//

#ifndef GRAV_SIM_CPU_CONFIG_HPP
#define GRAV_SIM_CPU_CONFIG_HPP

#include <glm/glm.hpp>

#include "colormap.hpp"

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

extern bool g_useColorMap;
extern float g_colorMapMaxVel;
extern float g_colorMapMaxSqrVel;

void loadSimulationFile(const char* simulationPath);

#endif //GRAV_SIM_CPU_CONFIG_HPP