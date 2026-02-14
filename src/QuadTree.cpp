//
// Created by kassie on 31/01/2026.
//

#include "QuadTree.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <glm/gtx/norm.hpp>

static constexpr long double QUADTREE_RESERVE_MULTIPLIER = 2.5L;
static constexpr float PRECOMPUTED_BOUNDS_MIN_SIZE = 1.0f;

static constexpr Color QUADTREE_VIS_FILL_COLOR = {0, 255, 255, 5};
static constexpr Color QUADTREE_VIS_OUTLINE_COLOR = {255, 255, 255, 50};
static constexpr Color QUADTREE_VIS_LEAF_OUTLINE_COLOR = RED;
static constexpr float QUADTREE_VIS_LINE_THICKNESS = 1.0f;

QuadTree::QuadTree(const std::vector<Body>& bodies)
	: m_bodies(&bodies) { }

void QuadTree::buildTree()
{
	m_nodes.clear();
	m_precomputedBoundsSizes.clear();
	m_nodeCounter = 0;
	m_boundsSize = 0;
	m_boundsCenter = {};

	if (m_indices.empty())
	{
		m_indices.resize(m_bodies->size());
		std::iota(m_indices.begin(), m_indices.end(), 0);
	}

	const auto reserveSize = static_cast<size_t>(QUADTREE_RESERVE_MULTIPLIER * m_bodies->size());
	m_nodes.resize(reserveSize);

	calculateBoundingSquare();

	buildTree(m_indices.begin(), m_indices.end(), m_boundsSize, m_boundsCenter);

	float precomputedBoundsSize = m_boundsSize;
	while (precomputedBoundsSize > PRECOMPUTED_BOUNDS_MIN_SIZE)
	{
		m_precomputedBoundsSizes.push_back(precomputedBoundsSize);
		precomputedBoundsSize /= 2;
	}
}

glm::vec2 QuadTree::accelAt(const glm::vec2 position) const
{
	return accelAt(position, 0, 0);
}

void QuadTree::visualize(const float cameraZoom) const
{
	const float halfSize = m_boundsSize / 2.0f;
	visualize(0,
		{m_boundsCenter.x - halfSize, m_boundsCenter.y - halfSize, m_boundsSize, m_boundsSize},
		cameraZoom);
}

void QuadTree::calculateBoundingSquare()
{
	glm::vec2 min = {INFINITY, INFINITY};
	glm::vec2 max = {-INFINITY, -INFINITY};

	for (const auto& body : *m_bodies)
	{
		if (body.position.x < min.x)
			min.x = body.position.x;
		if (body.position.x > max.x)
			max.x = body.position.x;
		if (body.position.y < min.y)
			min.y = body.position.y;
		if (body.position.y > max.y)
			max.y = body.position.y;
	}

	const float width = max.x - min.x;
	const float height = max.y - min.y;

	m_boundsSize = std::max(width, height);
	m_boundsCenter =  min + (max - min) / 2.0f;
}

NodeIndex_t QuadTree::buildTree(const IndexIt_t begin, const IndexIt_t end, const float size, const glm::vec2 center)
{
	if (begin == end)
		return NULL_INDEX;

	const NodeIndex_t result = m_nodeCounter++;
	Node& node = m_nodes[result];
	const long nodeLength = end - begin;

	glm::vec2 momentSum = {};
	float massSum = 0;

	for (auto it = begin; it != end; ++it)
	{
		const float mass = (*m_bodies)[*it].mass;
		momentSum += (*m_bodies)[*it].position * mass;
		massSum += mass;
	}

	node.com.position = momentSum / massSum;
	node.com.mass = massSum;

	if (nodeLength == 1)
	{
		node.bodyIndex = *begin;
		return result;
	}

	auto top  = [this, center](const BodyIndex_t index) { return (*m_bodies)[index].position.y < center.y; };
	auto left = [this, center](const BodyIndex_t index) { return (*m_bodies)[index].position.x < center.x; };

	const auto ySplit = std::partition(begin, end, top);
	const auto xSplitUpper = std::partition(begin, ySplit, left);
	const auto xSplitLower = std::partition(ySplit, end, left);

	const float halfSize = size / 2.0f;
	const float quarterSize = halfSize / 2.0f;

	node.child1 = buildTree(begin, xSplitUpper, halfSize,
		{center.x - quarterSize, center.y - quarterSize});
	node.child2 = buildTree(xSplitUpper, ySplit, halfSize,
		{center.x + quarterSize, center.y - quarterSize});
	node.child3 = buildTree(ySplit, xSplitLower, halfSize,
		{center.x - quarterSize, center.y + quarterSize});
	node.child4 = buildTree(xSplitLower, end, halfSize,
		{center.x + quarterSize, center.y + quarterSize});

	return result;
}

static glm::vec2 gravAccel(const glm::vec2 position, const glm::vec2 sourcePosition, const float sourceMass)
{
	const glm::vec2 rel = sourcePosition - position;
	const float sqrDist = glm::length2(rel);

	if (sqrDist == 0.0f)
		return {};

	const glm::vec2 dir = rel / sqrtf(sqrDist);

	return dir * GRAV_CONST * sourceMass / (GRAV_SMOOTHNESS + sqrDist);
}

static glm::vec2 gravAccel(const glm::vec2 rel, const float sqrDist, const float sourceMass)
{
	const glm::vec2 dir = rel / sqrtf(sqrDist);

	return dir * GRAV_CONST * sourceMass / (GRAV_SMOOTHNESS + sqrDist);
}

glm::vec2 QuadTree::accelAt(const glm::vec2 position, const NodeIndex_t nodeIndex, const int depth) const
{
	const Node& node = m_nodes[nodeIndex];

	if (node.isLeaf())
	{
		if ((*m_bodies)[node.bodyIndex].position == position)
			return {};

		return gravAccel(position, node.com.position, node.com.mass);
	}

	const glm::vec2 rel = node.com.position - position;
	const float sqrDist = glm::length2(rel);

	if (sqrDist == 0.0f)
		return {};

	const float boundsSize = m_precomputedBoundsSizes[depth];
	const float sqrBoundsSize = boundsSize * boundsSize;
	const float sqrHeuristic = sqrBoundsSize / sqrDist;

	if (sqrHeuristic < THETA * THETA)
		return gravAccel(rel, sqrDist, node.com.mass);

	glm::vec2 accelSum = {};

	if (node.child1 != NULL_INDEX)
		accelSum += accelAt(position, node.child1, depth + 1);
	if (node.child2 != NULL_INDEX)
		accelSum += accelAt(position, node.child2, depth + 1);
	if (node.child3 != NULL_INDEX)
		accelSum += accelAt(position, node.child3, depth + 1);
	if (node.child4 != NULL_INDEX)
		accelSum += accelAt(position, node.child4, depth + 1);

	return accelSum;
}

void QuadTree::visualize(const NodeIndex_t nodeIndex, const Rectangle rect, const float cameraZoom) const
{
	DrawRectangleRec(rect, QUADTREE_VIS_FILL_COLOR);
	const Node& node = m_nodes[nodeIndex];

	if (node.isLeaf())
	{
		DrawRectangleLinesEx(rect, QUADTREE_VIS_LINE_THICKNESS / cameraZoom, QUADTREE_VIS_LEAF_OUTLINE_COLOR);
		return;
	}

	DrawRectangleLinesEx(rect, QUADTREE_VIS_LINE_THICKNESS / cameraZoom, QUADTREE_VIS_OUTLINE_COLOR);

	if (node.child1 != NULL_INDEX)
		visualize(node.child1,
			{rect.x, rect.y, rect.width / 2.0f, rect.height / 2.0f}, cameraZoom);
	if (node.child2 != NULL_INDEX)
		visualize(node.child2,
			{rect.x + rect.width / 2.0f, rect.y, rect.width / 2.0f, rect.height / 2.0f}, cameraZoom);
	if (node.child3 != NULL_INDEX)
		visualize(node.child3,
			{rect.x, rect.y + rect.height / 2.0f, rect.width / 2.0f, rect.height / 2.0f}, cameraZoom);
	if (node.child4 != NULL_INDEX)
		visualize(node.child4,
			{rect.x + rect.width / 2.0f, rect.y + rect.height / 2.0f,
				rect.width / 2.0f, rect.height / 2.0f}, cameraZoom);
}
