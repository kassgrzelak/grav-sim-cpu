//
// Created by kassie on 17/02/2026.
//

#include "parameters.hpp"

#include <format>
#include <fstream>
#include <iostream>
#include <sstream>

float g_theta;
float g_gravConst;
float g_gravSmoothness;
glm::vec2 g_screenDims;
bool g_resizable;
int g_targetFPS;
float g_timeScale;
float g_deltaTime;
Color g_bodyColor;

void loadSimulationFile(const char* simulationPath)
{
    std::ifstream file(simulationPath);
    if (!file.is_open())
        throw std::runtime_error(std::format("Failed to open simulation config file given path {}.", simulationPath));

    bool thetaFound = false;
    bool gravConstFound = false;
    bool gravSmoothnessFound = false;
    bool screenDimsFound = false;
    bool resizableFound = false;
    bool targetFPSFound = false;
    bool timeScaleFound = false;
    bool bodyColorFound = false;

    int lineNum = 0;
    std::string line;
    while (std::getline(file, line))
    {
        ++lineNum;

        if (line.empty() || line[0] == '#')
            continue;

        std::stringstream ss(line);

        std::string parameter;
        ss >> parameter;

// Thanks to Robert Nystrom for teaching me this freaky macro trick!
#define READ_PARAMETER(varName, foundVar, var) \
    do \
    { \
        if (foundVar) \
            throw std::runtime_error(std::format("Double definition of {} on line {}.", varName, line)); \
        ss >> var; \
        foundVar = true; \
    } \
    while (false)
        if (parameter == "THETA")
            READ_PARAMETER("THETA", thetaFound, g_theta);
        else if (parameter == "GRAVCONST")
            READ_PARAMETER("GRAVCONST", gravConstFound, g_gravConst);
        else if (parameter == "GRAVSMOOTHNESS")
            READ_PARAMETER("GRAVSMOOTHNESS", gravSmoothnessFound, g_gravSmoothness);
        else if (parameter == "SCREENDIMS")
        {
            if (screenDimsFound)
                throw std::runtime_error(std::format("Double definition of SCREENDIMS on line {}.", line));

            float width, height;
            ss >> width >> height;
            g_screenDims = {width, height};

            screenDimsFound = true;
        }
        else if (parameter == "RESIZABLE")
            READ_PARAMETER("RESIZABLE", resizableFound, g_resizable);
        else if (parameter == "TARGETFPS")
            READ_PARAMETER("TARGETFPS", targetFPSFound, g_targetFPS);
        else if (parameter == "TIMESCALE")
            READ_PARAMETER("TIMESCALE", timeScaleFound, g_timeScale);
        else if (parameter == "BODYCOLOR")
        {
            if (bodyColorFound)
                throw std::runtime_error(std::format("Double definition of BODYCOLOR on line {}.", line));

            int r, g, b, a;
            ss >> r >> g >> b >> a;
            g_bodyColor = {static_cast<unsigned char>(r), static_cast<unsigned char>(g),
                static_cast<unsigned char>(b), static_cast<unsigned char>(a)};

            bodyColorFound = true;
        }
        else
            throw std::runtime_error(std::format("Unknown parameter '{}' on line {}.", parameter, lineNum));
#undef READ_PARAMETER

        if (ss.fail())
            throw std::runtime_error(std::format("Failed reading value on line {}. Check you have "
                "the right type/number of values.", lineNum));
    }

    if (!(thetaFound && gravConstFound && gravSmoothnessFound && screenDimsFound && targetFPSFound && timeScaleFound && bodyColorFound))
        throw std::runtime_error(std::format("Did not find a definition for every"));

    g_deltaTime = g_timeScale / g_targetFPS;
}
