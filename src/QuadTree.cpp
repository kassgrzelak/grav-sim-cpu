//
// Created by kassie on 31/01/2026.
//

#include "QuadTree.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>

static constexpr long double QUADTREE_RESERVE_MULTIPLIER = 4;

static constexpr Color QUADTREE_VIS_FILL_COLOR = {0, 255, 255, 5};
static constexpr Color QUADTREE_VIS_OUTLINE_COLOR = {255, 255, 255, 50};
static constexpr Color QUADTREE_VIS_LEAF_OUTLINE_COLOR = RED;
static constexpr float QUADTREE_VIS_LINE_THICKNESS = 1.0f;

QuadTree::QuadTree(const std::vector<glm::vec2>& positions, const std::vector<float>& masses)
	: m_positions(&positions), m_masses(&masses) { }

void QuadTree::buildTree()
{
	m_nodes.clear();
	m_coms.clear();
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
	m_coms.resize(reserveSize);

	calculateBoundingSquare();

	buildTree(m_indices.begin(), m_indices.end(), m_boundsSize, m_boundsCenter);
}

glm::vec2 QuadTree::accelAt(const glm::vec2 pos) const
{
	return {};
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

	// TODO: Mutex and make this parallel?
	for (const auto& pos : *m_positions)
	{
		if (pos.x < min.x)
			min.x = pos.x;
		if (pos.x > max.x)
			max.x = pos.x;
		if (pos.y < min.y)
			min.y = pos.y;
		if (pos.y > max.y)
			max.y = pos.y;
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
	CoM& com = m_coms[result];
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
		return result;

	// TODO: Check for bodies in identical positions.

	auto top  = [this, center](const BodyIndex_t index) { return (*m_positions)[index].y < center.y; };
	auto left = [this, center](const BodyIndex_t index) { return (*m_positions)[index].x < center.x; };

	const auto ySplit = std::partition(begin, end, top);
	const auto xSplitUpper = std::partition(begin, ySplit, left);
	const auto xSplitLower = std::partition(ySplit, end, left);

	// Exit early if
	if (xSplitUpper == begin && xSplitLower == end)
		return result;

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

void QuadTree::visualize(const NodeIndex_t nodeIndex, const Rectangle rect, const float cameraZoom) const
{
	DrawRectangleRec(rect, QUADTREE_VIS_FILL_COLOR);
	const Node& node = m_nodes[nodeIndex];

	if (isLeaf(node))
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

bool QuadTree::isLeaf(const Node& node)
{
	return node.child1 == NULL_INDEX &&
		   node.child2 == NULL_INDEX &&
		   node.child3 == NULL_INDEX &&
		   node.child4 == NULL_INDEX;
}
