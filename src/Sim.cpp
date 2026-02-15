//
// Created by kassie on 31/01/2026.
//

#include "Sim.hpp"

#include <algorithm>
#include <execution>
#include <format>

#include "BodyGenerator.hpp"

static constexpr float CAMERA_ZOOM_BUTTON_SPEED = 0.05f;
static constexpr float CAMERA_ZOOM_SCROLL_SPEED = 0.15f;

static constexpr float CAMERA_MAX_ZOOM = 12.0f;
static constexpr float CAMERA_MIN_ZOOM = 0.1f;

Sim::Sim() : m_quadTree(m_bodies), m_circleTex(), m_camera()
{
	BodyGenerator::generateBodies(m_bodies);

	m_quadTree.buildTree();
	initializeVelocities();

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(static_cast<int>(g_screenDims.x), static_cast<int>(g_screenDims.y), "CPU Gravity Simulation");
	SetTargetFPS(TARGET_FPS);

	m_circleTex = LoadTexture("assets/circle.png");

	m_camera.target = {g_screenCenter.x, g_screenCenter.y};
	m_camera.offset = {g_screenCenter.x, g_screenCenter.y};
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
	for (auto& body : m_bodies)
		body.velocity += m_quadTree.accelAt(body.position) * DELTA_TIME * 0.5f;
}

void Sim::updateScreenDims()
{
	if (IsWindowResized())
	{
		g_screenDims.x = static_cast<float>(GetScreenWidth());
		g_screenDims.y = static_cast<float>(GetScreenHeight());
		g_screenCenter = g_screenDims / 2.0f;
		m_camera.offset = {g_screenCenter.x, g_screenCenter.y};
	}
}

void Sim::takeInput()
{
	// Camera.
	{
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

		if (m_camera.zoom > CAMERA_MAX_ZOOM)
			m_camera.zoom = CAMERA_MAX_ZOOM;
		if (m_camera.zoom < CAMERA_MIN_ZOOM)
			m_camera.zoom = CAMERA_MIN_ZOOM;
	}

	// Toggles.
#define TOGGLE(key, var) \
	if (IsKeyPressed(key)) \
		var = !var
	{
		if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_P))
			m_paused = !m_paused;
		TOGGLE(KEY_Q, m_visualizeQuadTree);
		TOGGLE(KEY_D, m_showDetails);
		TOGGLE(KEY_R, m_timeReverse);
		TOGGLE(KEY_C, m_showControls);
	}
#undef TOGGLE
}

void Sim::update()
{
	if (m_timeReverse)
	{
		std::for_each(std::execution::par_unseq, m_bodies.begin(), m_bodies.end(),
		   [&](Body& body)
		   {
			   body.position -= body.velocity * DELTA_TIME;
		   });

		m_quadTree.buildTree();

		std::for_each(std::execution::par_unseq, m_bodies.begin(), m_bodies.end(),
		   [&](Body& body)
		   {
			   body.velocity -= m_quadTree.accelAt(body.position) * DELTA_TIME;
		   });
	}
	else
	{
		std::for_each(std::execution::par_unseq, m_bodies.begin(), m_bodies.end(),
		   [&](Body& body)
		   {
			   body.velocity += m_quadTree.accelAt(body.position) * DELTA_TIME;
		   	   body.position += body.velocity * DELTA_TIME;
		   });

		m_quadTree.buildTree();
	}

}

void Sim::draw() const
{
	BeginDrawing();
	ClearBackground(BLACK);

	BeginMode2D(m_camera);
	for (auto& body : m_bodies)
	{
		const float radius = body.diameter / 2.0f;

		DrawTexturePro(
		   m_circleTex,
		   { 0, 0, static_cast<float>(m_circleTex.width), static_cast<float>(m_circleTex.height) },
		   { body.position.x, body.position.y, body.diameter, body.diameter },
		   { radius, radius },
		   0.0f,
		   BODY_COLOR
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

	DRAW_DETAIL("Delta time", DELTA_TIME);
	DRAW_DETAIL("Timescale", TIME_SCALE);
	DRAW_DETAIL("Target FPS", TARGET_FPS);
	DRAW_DETAIL("Theta", THETA);
	DRAW_DETAIL("N", m_bodies.size());

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
