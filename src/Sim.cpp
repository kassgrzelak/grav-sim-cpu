//
// Created by kassie on 31/01/2026.
//

#include "Sim.hpp"

#include <exception>

#include "BodyGenerator.hpp"

static constexpr float CAMERA_SPEED = 400.0f;

static constexpr float CAMERA_ZOOM_BUTTON_SPEED = 0.05f;
static constexpr float CAMERA_ZOOM_SCROLL_SPEED = 0.15f;

static constexpr float CAMERA_MAX_ZOOM = 3.0f;
static constexpr float CAMERA_MIN_ZOOM = 0.3f;

Sim::Sim() : m_quadTree(m_positions, m_masses), m_circleTex(), m_camera()
{
	BodyGenerator::generateBodies(m_positions, m_velocities, m_masses, m_diameters);

	assert(m_positions.size() == m_velocities.size() && m_velocities.size() == m_masses.size());
	m_bodyNum = m_positions.size();

	m_quadTree.buildTree();
	initializeVelocities();

	InitWindow(SCREEN_DIMS.x, SCREEN_DIMS.y, "CPU Gravity Simulation");
	SetTargetFPS(TARGET_FPS);

	m_circleTex = LoadTexture("assets/circle.png");

	m_camera.target = {SCREEN_CENTER.x, SCREEN_CENTER.y};
	m_camera.offset = {SCREEN_CENTER.x, SCREEN_CENTER.y};
	m_camera.rotation = 0.0f;
	m_camera.zoom = 1.0f;
}

void Sim::run()
{
	while (!WindowShouldClose())
	{
		takeInput();
		if (!m_paused)
			update();
		draw();
	}
}

void Sim::initializeVelocities()
{
	for (BodyIndex_t i = 0; i < m_bodyNum; ++i)
		m_velocities[i] += m_quadTree.accelAt(m_positions[i]) * DELTA_TIME * 0.5f;
}

void Sim::takeInput()
{
	const float dt = GetFrameTime();

	// Camera.
	{
		const float sprintMultiplier = IsKeyDown(KEY_LEFT_SHIFT) ? 2.0f : 1.0f;

		if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
			m_camera.target.x -= sprintMultiplier * CAMERA_SPEED / m_camera.zoom * dt;
		if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
			m_camera.target.x += sprintMultiplier * CAMERA_SPEED / m_camera.zoom * dt;
		if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
			m_camera.target.y -= sprintMultiplier * CAMERA_SPEED / m_camera.zoom * dt;
		if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
			m_camera.target.y += sprintMultiplier * CAMERA_SPEED / m_camera.zoom * dt;

		if (IsKeyDown(KEY_MINUS))
			m_camera.zoom = expf(logf(m_camera.zoom) - CAMERA_ZOOM_BUTTON_SPEED);
		if (IsKeyDown(KEY_EQUAL))
			m_camera.zoom = expf(logf(m_camera.zoom) + CAMERA_ZOOM_BUTTON_SPEED);

		const float mouseWheelDelta = GetMouseWheelMoveV().y;
		if (mouseWheelDelta != 0)
			m_camera.zoom = expf(logf(m_camera.zoom) + CAMERA_ZOOM_SCROLL_SPEED * mouseWheelDelta);

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			const auto [dx, dy] = GetMouseDelta();
			m_camera.target.x -= dx / m_camera.zoom;
			m_camera.target.y -= dy / m_camera.zoom;
		}

		if (m_camera.zoom > CAMERA_MAX_ZOOM)
			m_camera.zoom = CAMERA_MAX_ZOOM;
		if (m_camera.zoom < CAMERA_MIN_ZOOM)
			m_camera.zoom = CAMERA_MIN_ZOOM;
	}

	// Pause.
	{
		if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_P))
			m_paused = !m_paused;
	}

	// Quadtree visualization.
	{
		if (IsKeyPressed(KEY_Q))
			m_visualizeQuadTree = !m_visualizeQuadTree;
	}
}

void Sim::update()
{
	m_quadTree.buildTree();
}

void Sim::draw() const
{
	BeginDrawing();
	ClearBackground(BLACK);

	BeginMode2D(m_camera);
	for (BodyIndex_t i = 0; i < m_bodyNum; ++i)
	{
		const glm::vec2 position = m_positions[i];
		const float diameter = m_diameters[i];
		const float radius = diameter / 2.0f;

		DrawTexturePro(
		   m_circleTex,
		   { 0, 0, static_cast<float>(m_circleTex.width), static_cast<float>(m_circleTex.height) },
		   { position.x, position.y, diameter, diameter },
		   { radius, radius },
		   0.0f,
		   BODY_COLOR
	   );
	}

	if (m_visualizeQuadTree)
		m_quadTree.visualize(m_camera.zoom);
	EndMode2D();

	DrawFPS(10, 10);
	EndDrawing();
}
