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

QuadTree::QuadTree(const std::vector<glm::vec2>& positions, const std::vector<float>& masses)
	: m_positions(&positions), m_masses(&masses) { }

void QuadTree::buildTree()
{
	m_nodes.clear();
	m_nodeCoMs.clear();
	m_nodeBodyIndices.clear();
	m_nodeIsLeaf.clear();
	m_precomputedBoundsSizes.clear();
	m_nodeCounter = 0;
	m_boundsSize = 0;
	m_boundsCenter = {};

	if (m_indices.empty())
	{
		m_indices.resize(m_positions->size());
		std::iota(m_indices.begin(), m_indices.end(), 0);
	}

	const auto reserveSize = static_cast<size_t>(QUADTREE_RESERVE_MULTIPLIER * m_positions->size());
	m_nodes.resize(reserveSize);
	m_nodeCoMs.resize(reserveSize);
	m_nodeBodyIndices.resize(reserveSize);
	m_nodeIsLeaf.resize(reserveSize);

	calculateBoundingSquare();

	buildTree(m_indices.begin(), m_indices.end(), m_boundsSize, m_boundsCenter);

	float precomputedBoundsSize = m_boundsSize;
	while (precomputedBoundsSize > PRECOMPUTED_BOUNDS_MIN_SIZE)
	{
		m_precomputedBoundsSizes.push_back(precomputedBoundsSize);
		precomputedBoundsSize /= 2;
	}
}

const std::vector<BodyIndex_t>& QuadTree::getIndices() const
{
	return m_indices;
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

	for (const auto& position : *m_positions)
	{
		if (position.x < min.x)
			min.x = position.x;
		if (position.x > max.x)
			max.x = position.x;
		if (position.y < min.y)
			min.y = position.y;
		if (position.y > max.y)
			max.y = position.y;
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
	CoM& com = m_nodeCoMs[result];
	const long nodeLength = end - begin;

	glm::vec2 momentSum = {};
	float massSum = 0;

	for (auto it = begin; it != end; ++it)
	{
		const float mass = (*m_masses)[*it];
		momentSum += (*m_positions)[*it] * mass;
		massSum += mass;
	}

	com.position = momentSum / massSum;
	com.mass = massSum;

	if (nodeLength == 1)
	{
		m_nodeBodyIndices[result] = *begin;
		m_nodeIsLeaf[result] = true;
		return result;
	}

	m_nodeBodyIndices[result] = NULL_INDEX;
	m_nodeIsLeaf[result] = false;

	auto top  = [this, center](const BodyIndex_t index) { return (*m_positions)[index].y < center.y; };
	auto left = [this, center](const BodyIndex_t index) { return (*m_positions)[index].x < center.x; };

	const auto ySplit = std::partition(begin, end, top);
	const auto xSplitUpper = std::partition(begin, ySplit, left);
	const auto xSplitLower = std::partition(ySplit, end, left);

	auto& [child1, child2, child3, child4] = m_nodes[result];
	const float halfSize = size / 2.0f;
	const float quarterSize = halfSize / 2.0f;

	child1 = buildTree(begin, xSplitUpper, halfSize,
		{center.x - quarterSize, center.y - quarterSize});
	child2 = buildTree(xSplitUpper, ySplit, halfSize,
		{center.x + quarterSize, center.y - quarterSize});
	child3 = buildTree(ySplit, xSplitLower, halfSize,
		{center.x - quarterSize, center.y + quarterSize});
	child4 = buildTree(xSplitLower, end, halfSize,
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
	const CoM& com = m_nodeCoMs[nodeIndex];
	const Node& node = m_nodes[nodeIndex];

	if (m_nodeIsLeaf[nodeIndex])
	{
		if ((*m_positions)[m_nodeBodyIndices[nodeIndex]] == position)
			return {};

		return gravAccel(position, com.position, com.mass);
	}

	const glm::vec2 rel = com.position - position;
	const float sqrDist = glm::length2(rel);

	if (sqrDist == 0.0f)
		return {};

	const float boundsSize = m_precomputedBoundsSizes[depth];
	const float sqrBoundsSize = boundsSize * boundsSize;
	const float sqrHeuristic = sqrBoundsSize / sqrDist;

	if (sqrHeuristic < THETA * THETA)
		return gravAccel(rel, sqrDist, com.mass);

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

	if (m_nodeIsLeaf[nodeIndex])
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
