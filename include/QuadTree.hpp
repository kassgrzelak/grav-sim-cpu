//
// Created by kassie on 31/01/2026.
//

#ifndef GRAV_SIM_CPU_QUAD_TREE_HPP
#define GRAV_SIM_CPU_QUAD_TREE_HPP
#include <vector>
#include <glm/vec2.hpp>

#include "CoM.hpp"
#include "common.hpp"
#include "config.hpp"

using NodeIndex_t = uint32_t;
static constexpr NodeIndex_t NULL_INDEX = -1;

class QuadTree
{
public:
	QuadTree(const std::vector<glm::vec2>& positions, const std::vector<float>& masses);

	void buildTree();

	[[nodiscard]] const std::vector<BodyIndex_t>& getIndices() const;

	[[nodiscard]] glm::vec2 accelAt(glm::vec2 position) const;

	void visualize(float cameraZoom) const;

private:
	struct Node
	{
		NodeIndex_t child1 = NULL_INDEX;
		NodeIndex_t child2 = NULL_INDEX;
		NodeIndex_t child3 = NULL_INDEX;
		NodeIndex_t child4 = NULL_INDEX;
	};

	std::vector<BodyIndex_t> m_indices;
	const std::vector<glm::vec2>* m_positions;
	const std::vector<float>* m_masses;

	std::vector<Node> m_nodes;
	std::vector<CoM> m_nodeComs;
	std::vector<BodyIndex_t> m_nodeBodyIndices;
	std::vector<uint8_t> m_nodeIsLeaf;

	std::vector<float> m_precomputedBoundsSizes;

	NodeIndex_t m_nodeCounter = 0;
	float m_boundsSize = 0;
	glm::vec2 m_boundsCenter = {};

	void calculateBoundingSquare();

	NodeIndex_t buildTree(IndexIt_t begin, IndexIt_t end, float size, glm::vec2 center);

	glm::vec2 accelAt(glm::vec2 position, NodeIndex_t nodeIndex, int depth) const;

	void visualize(NodeIndex_t nodeIndex, Rectangle rect, float cameraZoom) const;
};

#endif //GRAV_SIM_CPU_QUAD_TREE_HPP