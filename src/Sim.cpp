//
// Created by kassie on 31/01/2026.
//

#include "Sim.hpp"

#include <algorithm>
#include <execution>
#include <format>
#include <glm/gtx/norm.hpp>

#include "BodyGenerator.hpp"

static constexpr float CAMERA_ZOOM_BUTTON_SPEED = 0.05f;
static constexpr float CAMERA_ZOOM_SCROLL_SPEED = 0.15f;
static constexpr float CAMERA_MAX_ZOOM = 12.0f;
static constexpr float CAMERA_MIN_ZOOM = 0.1f;

static constexpr float MIN_TIMESCALE = 1.0f / 64.0f;
static constexpr float MAX_TIMESCALE = 8;

Sim::Sim(const char* generationPath) : m_quadTree(m_positions, m_masses), m_circleTex(), m_camera()
{
	BodyGenerator::generateBodies(generationPath, m_positions, m_velocities, m_masses, m_diameters);

	assert(m_positions.size() == m_velocities.size() && m_velocities.size() == m_masses.size());

	m_quadTree.buildTree();
	initializeVelocities();

	if (g_resizable)
		SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTraceLogLevel(LOG_ERROR); // Suppress Raylib logs.
	InitWindow(static_cast<int>(g_screenDims.x), static_cast<int>(g_screenDims.y), "CPU Gravity Simulation");
	SetTargetFPS(g_targetFPS);

	m_circleTex = LoadTexture("assets/circle.png");

	m_camera.target = {0, 0};
	m_camera.offset = {g_screenDims.x / 2.0f, g_screenDims.y / 2.0f};
	m_camera.rotation = 0.0f;
	m_camera.zoom = 1.0f;
}

void Sim::run()
{
	while (!WindowShouldClose())
	{
		updateScreenDims();
		takeInput();
		if (!m_paused)
			update();
		draw();
	}
}

void Sim::initializeVelocities()
{
	for (BodyIndex_t i = 0; i < m_positions.size(); ++i)
		m_velocities[i] += m_quadTree.accelAt(m_positions[i]) * g_deltaTime * 0.5f;
}

void Sim::updateScreenDims()
{
	if (IsWindowResized())
	{
		g_screenDims.x = static_cast<float>(GetScreenWidth());
		g_screenDims.y = static_cast<float>(GetScreenHeight());
		m_camera.offset = {g_screenDims.x / 2.0f, g_screenDims.y / 2.0f};
	}
}

void Sim::takeInput()
{
	// Camera.
	if (IsKeyDown(KEY_MINUS))
		m_camera.zoom = expf(logf(m_camera.zoom) - CAMERA_ZOOM_BUTTON_SPEED);
	if (IsKeyDown(KEY_EQUAL))
		m_camera.zoom = expf(logf(m_camera.zoom) + CAMERA_ZOOM_BUTTON_SPEED);

	const float mouseWheelDelta = GetMouseWheelMoveV().y;
	if (mouseWheelDelta != 0)
	{
		const Vector2 mouseWorldBefore = GetScreenToWorld2D(GetMousePosition(), m_camera);

		m_camera.zoom = expf(logf(m_camera.zoom) + CAMERA_ZOOM_SCROLL_SPEED * mouseWheelDelta);

		if (m_camera.zoom >= CAMERA_MIN_ZOOM && m_camera.zoom <= CAMERA_MAX_ZOOM)
		{
			const Vector2 mouseWorldAfter = GetScreenToWorld2D(GetMousePosition(), m_camera);

			m_camera.target.x += mouseWorldBefore.x - mouseWorldAfter.x;
			m_camera.target.y += mouseWorldBefore.y - mouseWorldAfter.y;
		}
	}

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		const auto [dx, dy] = GetMouseDelta();
		m_camera.target.x -= dx / m_camera.zoom;
		m_camera.target.y -= dy / m_camera.zoom;
	}

	if (IsKeyPressed(KEY_F))
	{
		const glm::vec2 CoMPosition = m_quadTree.getSystemCoMPosition();
		m_camera.target.x = CoMPosition.x;
		m_camera.target.y = CoMPosition.y;
	}

	if (m_camera.zoom > CAMERA_MAX_ZOOM)
		m_camera.zoom = CAMERA_MAX_ZOOM;
	if (m_camera.zoom < CAMERA_MIN_ZOOM)
		m_camera.zoom = CAMERA_MIN_ZOOM;

	// Time scale.
	if (IsKeyPressed(KEY_COMMA))
		g_timeScale /= 2.0f;
	if (IsKeyPressed(KEY_PERIOD))
		g_timeScale *= 2.0f;

	if (g_timeScale > MAX_TIMESCALE)
		g_timeScale = MAX_TIMESCALE;
	if (g_timeScale < MIN_TIMESCALE)
		g_timeScale = MIN_TIMESCALE;

	g_deltaTime = g_timeScale / static_cast<float>(g_targetFPS);

	// Toggles.
#define TOGGLE(key, var) \
	if (IsKeyPressed(key)) \
		var = !var

	if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_P))
		m_paused = !m_paused;
	TOGGLE(KEY_Q, m_visualizeQuadTree);
	TOGGLE(KEY_D, m_showDetails);
	TOGGLE(KEY_R, m_timeReverse);
	TOGGLE(KEY_C, m_showControls);
#undef TOGGLE
}

void Sim::update()
{
	const auto& indices = m_quadTree.getIndices();

	if (m_timeReverse)
	{
		std::for_each(std::execution::par_unseq, indices.begin(), indices.end(),
		   [&](const BodyIndex_t index)
		   {
			   m_positions[index] -= m_velocities[index] * g_deltaTime;
		   });

		m_quadTree.buildTree();

		std::for_each(std::execution::par_unseq, indices.begin(), indices.end(),
		   [&](const BodyIndex_t index)
		   {
			   m_velocities[index] -= m_quadTree.accelAt(m_positions[index]) * g_deltaTime;
		   });
	}
	else
	{
		std::for_each(std::execution::par_unseq, indices.begin(), indices.end(),
		   [&](const BodyIndex_t index)
		   {
			   m_velocities[index] += m_quadTree.accelAt(m_positions[index]) * g_deltaTime;
			   m_positions[index] += m_velocities[index] * g_deltaTime;
		   });

		m_quadTree.buildTree();
	}

}

void Sim::draw() const
{
	BeginDrawing();
	ClearBackground(BLACK);

	BeginMode2D(m_camera);
	for (BodyIndex_t i = 0; i < m_positions.size(); ++i)
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
		   g_bodyColor
	   );
	}

	if (m_visualizeQuadTree)
		m_quadTree.visualize(m_camera.zoom);
	EndMode2D();

	if (m_showDetails)
		drawDetails();

	if (m_showControls)
		drawControls();
	else
		drawTextRJust("Press C to show controls",
			static_cast<int>(g_screenDims.x - 5), static_cast<int>(g_screenDims.y - 25), 20, WHITE);

	DrawFPS(5, 5);
	EndDrawing();
}

void Sim::drawDetails() const
{
	int y = 5;

#define DRAW_DETAIL(name, value) \
	DrawText(std::format("{} = {}", name, value).c_str(), \
		5, static_cast<int>(g_screenDims.y - (y += 20)), 20, WHITE)

	DRAW_DETAIL("Delta time", g_deltaTime);
	DRAW_DETAIL("Timescale", g_timeScale);
	DRAW_DETAIL("Target FPS", g_targetFPS);
	DRAW_DETAIL("Theta", g_theta);
	DRAW_DETAIL("N", m_positions.size());

#undef DRAW_DETAIL
}

void Sim::drawControls()
{
	int y = 5;

#define DRAW_CONTROL(control, desc) \
	drawTextRJust(std::format("{}: {}", control, desc).c_str(), \
		static_cast<int>(g_screenDims.x - 5), static_cast<int>(g_screenDims.y - (y += 20)), 20, WHITE)

	DRAW_CONTROL("Q", "Quadtree visualization");
	DRAW_CONTROL("R", "Reverse time");
	DRAW_CONTROL("D", "Show sim details");
	DRAW_CONTROL("F", "Focus on system CoM");
	DRAW_CONTROL("Comma/period", "Change time scale");
	DRAW_CONTROL("Space or P", "Pause");
	DRAW_CONTROL("Scroll or +/-", "Zoom");
	DRAW_CONTROL("Click and drag", "Pan");

#undef DRAW_CONTROL
}

void Sim::drawTextRJust(const char* text, const int x, const int y, const int fontSize, const Color color)
{
	const int width = MeasureText(text, fontSize);
	DrawText(text, x - width, y, fontSize, color);
}
