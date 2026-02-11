//
// Created by kassie on 30/01/2026.
//

#ifndef GRAV_SIM_CPU_SIM_HPP
#define GRAV_SIM_CPU_SIM_HPP

#include <vector>
#include <glm/vec2.hpp>

#include "Body.hpp"
#include "config.hpp"
#include "QuadTree.hpp"

class Sim
{
public:
	Sim();

	void run();

private:
	std::vector<Body> m_bodies;

	QuadTree m_quadTree;

	Texture2D m_circleTex;
	Camera2D m_camera;

	bool m_paused = false;
	bool m_visualizeQuadTree = false;
	bool m_showDetails = false;
	bool m_timeReverse = false;
	bool m_showControls = false;

	void initializeVelocities();

	void updateScreenDims();
	void takeInput();
	void update();
	void draw() const;

	void drawDetails() const;
	static void drawControls() ;

	static void drawTextRJust(const char* text, int x, int y, int fontSize, Color color);
};

#endif //GRAV_SIM_CPU_SIM_HPP