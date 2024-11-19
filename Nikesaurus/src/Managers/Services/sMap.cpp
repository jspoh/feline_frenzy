/*****************************************************************//**
 * \file   sMap.cpp
 * \brief  Map manager
 *
 * \author Bryan Soh, 2301238, z.soh@digipen.edu (100%)
 * \date   November 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Managers/Services/sMap.h"

namespace NIKE {

	void Map::Service::init(Vector2i const& gridsize, Vector2f const& cellsize) {
		grid_size = gridsize;
		cell_size = cellsize;

		//Initialize grid
		grid.resize(grid_size.y);
		for (size_t i = 0; i < grid_size.y; i++) {
			grid.at(i).resize(grid_size.x);
		}
	}

	void Map::Service::setCellBlocked(size_t x, size_t y, bool blocked) {
		if (x < 0 || x >= grid_size.x || y < 0 || y >= grid_size.y) {
			throw std::out_of_range("Cell coordinates out of bounds");
		}
		grid.at(y).at(x).b_blocked = blocked;
	}

	bool Map::Service::isCellBlocked(size_t x, size_t y) const {
		if (x < 0 || x >= grid_size.x || y < 0 || y >= grid_size.y) {
			throw std::out_of_range("Cell coordinates out of bounds");
		}
		return grid.at(y).at(x).b_blocked;
	}

	void Map::Service::setGridSize(Vector2i const& size) {
		grid_size = size;

		//Initialize grid
		grid.resize(grid_size.y);
		for (size_t i = 0; i < grid_size.y; i++) {
			grid.at(i).resize(grid_size.x);
		}
	}

	Vector2i Map::Service::getGridSize() const {
		return grid_size;
	}

	void Map::Service::setCellSize(Vector2f const& size) {
		cell_size = size;
	}

	Vector2f Map::Service::getCellSize() const {
		return cell_size;
	}

  //  std::vector<NIKE::Math::Vector2f> Map::Service::findPath(NIKE::Math::Vector2f start, NIKE::Math::Vector2f goal) {
		//// Node structure representing each position in the grid for pathfinding
		//struct Node {
		//	NIKE::Math::Vector2f position;
		//	float gCost;  // Distance from the start node
		//	float hCost;  // Heuristic distance to the goal node
		//	float fCost() const { return gCost + hCost; }
		//};

		//// Custom comparator for priority queue to prioritize nodes with lower fCost
		//struct CompareNode {
		//	bool operator()(const Node& a, const Node& b) {
		//		return a.fCost() > b.fCost();
		//	}
		//};

		//// Heuristic function (Manhattan distance)
		//auto calculate_heuristic = [](NIKE::Math::Vector2f a, NIKE::Math::Vector2f b) {
		//	return std::abs(a.x - b.x) + std::abs(a.y - b.y);
		//	};

		//// Priority queue to store nodes to explore, prioritized by lowest fCost
		//std::priority_queue<Node, std::vector<Node>, CompareNode> open_list;
		//// Maps for reconstructing the path and tracking the cost from the start
		//std::unordered_map<int, NIKE::Math::Vector2f> came_from;
		//std::unordered_map<int, float> gScore;

		//// Helper function to convert a position to a unique hash for grid indexing
		//auto hash_position = [this](NIKE::Math::Vector2f pos) {
		//	int x = static_cast<int>(pos.x);
		//	int y = static_cast<int>(pos.y);
		//	return y * static_cast<int>(width) + x;
		//	};

		//// Initialize start node and insert it into the open list
		//open_list.push(Node{ start, 0.0f, calculate_heuristic(start, goal) });
		//gScore[hash_position(start)] = 0.0f;

		//// Possible movement directions (up, down, left, right)
		//std::vector<NIKE::Math::Vector2f> directions = { {0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, -1.0f}, {-1.0f, 0.0f} };

		//// A* search loop
		//while (!open_list.empty()) {
		//	Node current = open_list.top();
		//	open_list.pop();

		//	// If the goal is reached, reconstruct the path
		//	if (current.position == goal) {
		//		std::vector<NIKE::Math::Vector2f> path;
		//		for (NIKE::Math::Vector2f pos = goal; pos != start; pos = came_from[hash_position(pos)]) {
		//			path.push_back(pos);
		//		}
		//		path.push_back(start);
		//		std::reverse(path.begin(), path.end());
		//		return path;  // Return the found path
		//	}

		//	// Explore each direction
		//	for (const auto& dir : directions) {
		//		NIKE::Math::Vector2f neighbor_pos = current.position + dir;

		//		// Check boundaries and if cell is blocked
		//		if (neighbor_pos.x < 0 || neighbor_pos.x >= width ||
		//			neighbor_pos.y < 0 || neighbor_pos.y >= height ||
		//			isCellBlocked(neighbor_pos.x, neighbor_pos.y)) {
		//			continue;
		//		}

		//		// Calculate tentative gScore
		//		float tentativeGScore = gScore[hash_position(current.position)] + 1.0f;

		//		int neighbor_hash = hash_position(neighbor_pos);
		//		if (gScore.find(neighbor_hash) == gScore.end() || tentativeGScore < gScore[neighbor_hash]) {
		//			came_from[neighbor_hash] = current.position;
		//			gScore[neighbor_hash] = tentativeGScore;
		//			float hCost = calculate_heuristic(neighbor_pos, goal);
		//			open_list.push(Node{ neighbor_pos, tentativeGScore, hCost });
		//		}
		//	}
		//}

		//// Return empty if no path is found
		//return {};
  //  }
}