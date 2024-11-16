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
	void Map::Service::initializeGrid(float map_width, float map_height, const std::vector<std::vector<int>>& tileData) {
		// Set map width and height for the grid
		width = map_width;
		height = map_height;

		// Resize and reset the blockedCells grid
		blockedCells.resize(static_cast<size_t>(width) * static_cast<size_t>(height), false);

		// Loop through tileData to set blocked cells based on tileID
		for (int y = 0; y < map_height; ++y) {
			for (int x = 0; x < map_width; ++x) {
				int tileID = tileData[y][x];

				// Define blocked cells based on specific tile IDs
				if (tileID != 4) {
					setCellBlocked(static_cast<float>(x), static_cast<float>(y), true); // Mark cell as blocked
				}
			}
		}
	}

	bool Map::Service::isCellBlocked(float x, float y) const {
		if (x < 0 || x >= width || y < 0 || y >= height) {
			throw std::out_of_range("Cell coordinates out of bounds");
		}
		return blockedCells[static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x)];
	}

	void Map::Service::setCellBlocked(float x, float y, bool blocked) {
		if (x < 0 || x >= width || y < 0 || y >= height) {
			throw std::out_of_range("Cell coordinates out of bounds");
		}
		blockedCells[static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x)] = blocked;
	}

    std::vector<NIKE::Math::Vector2f> Map::Service::findPath(NIKE::Math::Vector2f start, NIKE::Math::Vector2f goal) {
		// Node structure representing each position in the grid for pathfinding
		struct Node {
			NIKE::Math::Vector2f position;
			float gCost;  // Distance from the start node
			float hCost;  // Heuristic distance to the goal node
			float fCost() const { return gCost + hCost; }
		};

		// Custom comparator for priority queue to prioritize nodes with lower fCost
		struct CompareNode {
			bool operator()(const Node& a, const Node& b) {
				return a.fCost() > b.fCost();
			}
		};

		// Heuristic function (Manhattan distance)
		auto calculate_heuristic = [](NIKE::Math::Vector2f a, NIKE::Math::Vector2f b) {
			return std::abs(a.x - b.x) + std::abs(a.y - b.y);
			};

		// Priority queue to store nodes to explore, prioritized by lowest fCost
		std::priority_queue<Node, std::vector<Node>, CompareNode> open_list;
		// Maps for reconstructing the path and tracking the cost from the start
		std::unordered_map<int, NIKE::Math::Vector2f> came_from;
		std::unordered_map<int, float> gScore;

		// Helper function to convert a position to a unique hash for grid indexing
		auto hash_position = [this](NIKE::Math::Vector2f pos) {
			int x = static_cast<int>(pos.x);
			int y = static_cast<int>(pos.y);
			return y * static_cast<int>(width) + x;
			};

		// Initialize start node and insert it into the open list
		open_list.push(Node{ start, 0.0f, calculate_heuristic(start, goal) });
		gScore[hash_position(start)] = 0.0f;

		// Possible movement directions (up, down, left, right)
		std::vector<NIKE::Math::Vector2f> directions = { {0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, -1.0f}, {-1.0f, 0.0f} };

		// A* search loop
		while (!open_list.empty()) {
			Node current = open_list.top();
			open_list.pop();

			// If the goal is reached, reconstruct the path
			if (current.position == goal) {
				std::vector<NIKE::Math::Vector2f> path;
				for (NIKE::Math::Vector2f pos = goal; pos != start; pos = came_from[hash_position(pos)]) {
					path.push_back(pos);
				}
				path.push_back(start);
				std::reverse(path.begin(), path.end());
				return path;  // Return the found path
			}

			// Explore each direction
			for (const auto& dir : directions) {
				NIKE::Math::Vector2f neighbor_pos = current.position + dir;

				// Check boundaries and if cell is blocked
				if (neighbor_pos.x < 0 || neighbor_pos.x >= width ||
					neighbor_pos.y < 0 || neighbor_pos.y >= height ||
					isCellBlocked(neighbor_pos.x, neighbor_pos.y)) {
					continue;
				}

				// Calculate tentative gScore
				float tentativeGScore = gScore[hash_position(current.position)] + 1.0f;

				int neighbor_hash = hash_position(neighbor_pos);
				if (gScore.find(neighbor_hash) == gScore.end() || tentativeGScore < gScore[neighbor_hash]) {
					came_from[neighbor_hash] = current.position;
					gScore[neighbor_hash] = tentativeGScore;
					float hCost = calculate_heuristic(neighbor_pos, goal);
					open_list.push(Node{ neighbor_pos, tentativeGScore, hCost });
				}
			}
		}

		// Return empty if no path is found
		return {};
    }

	/*
	void Map::Service::loadMapFromFile(const std::string& file, std::shared_ptr<NIKE::Scenes::Layer>& background_layer, std::shared_ptr<NIKE::Scenes::Layer>& player_layer, std::vector<std::vector<int>>& grid, const NIKE::Math::Vector2<float>& center) {
		// Open Scene file
		std::ifstream ifs{ file, std::ios::in };
		if (!ifs) {
			throw std::runtime_error("Failed to open mesh file: " + file);
		}

		// Read grid width and height
		int width, height;
		ifs >> width >> height;
		if (!ifs) {
			throw std::runtime_error("Failed to read grid dimensions from file: " + file);
		}

		// Create a grid to store tile information
		grid.resize(height, std::vector<int>(width));

		// Save each tile ID into the grid
		for (int row = 0; row < height; ++row) {
			for (int col = 0; col < width; ++col) {
				if (!(ifs >> grid[row][col])) {
					throw std::runtime_error("Failed to read tile data at row " + std::to_string(row) + ", column " + std::to_string(col));
				}
			}
		}

		// Initialize the grid for pathfinding using the loaded map data
		initializeGrid(width, height, grid);

		// Calculate offset to center the grid
		float tile_size = 100.0f;
		float offset_x = (width * tile_size) / 2.0f - center.x;
		float offset_y = (height * tile_size) / 2.0f - center.y;

		// Create entities from grid info
		for (int row = 0; row < height; ++row) {
			for (int col = 0; col < width; ++col) {
				// Create tile
				createTile(grid[row][col], row, col, tile_size, offset_x, offset_y, width, height, background_layer, player_layer);
			}
		}
	}

	void Map::Service::createTile(int tileID, int row, int col, float tile_size, float offset_x, float offset_y, int width, int height,
		std::shared_ptr<NIKE::Scenes::Layer>& background_layer,
		std::shared_ptr<NIKE::Scenes::Layer>& player_layer) {
		bool flip{ false };
		bool collide{ false };
		std::string texture_name{};

		switch (tileID) {
		case 1: texture_name = "wallTopCorner"; collide = true; break;
		case 2: texture_name = "wallTopMiddle"; collide = true; break;
		case 3: texture_name = "wallLeft"; collide = true; break;
		case 4: texture_name = "grass"; break;
		case 5: texture_name = "wallBottomCorner"; collide = true; break;
		case 6: texture_name = "wallBottomMiddle"; collide = true; break;
		case 7: texture_name = "wallTopCorner"; flip = true; collide = true; break;
		case 8: texture_name = "wallLeft"; flip = true; collide = true; break;
		case 9: texture_name = "wallBottomCorner"; flip = true; collide = true; break;
		default: texture_name = "grass"; break;
		}


		// Set layer to background if not collidable
		unsigned int layer = (collide) ? player_layer->getLayerID() : background_layer->getLayerID();
		NIKE::Entity::Type tile_entity = NIKE_ECS_SERVICE->createEntity(layer);

		NIKE_IMGUI_SERVICE->addEntityRef("tile_" + std::to_string(row) + "_" + std::to_string(col), tile_entity);
		NIKE_ECS_SERVICE->addEntityComponent<NIKE::Transform::Transform>(tile_entity,
			NIKE::Transform::Transform({ col * tile_size - offset_x, (height - 1 - row) * tile_size - offset_y },
				{ 100.0f, 100.0f }, 0.0f));

		if (collide) {
			NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Dynamics>(tile_entity, NIKE::Physics::Dynamics(200.0f, 1.0f, 0.1f));
			NIKE_ECS_SERVICE->addEntityComponent<NIKE::Physics::Collider>(tile_entity, NIKE::Physics::Collider(NIKE::Physics::Resolution::NONE));
		}

		NIKE_ECS_SERVICE->addEntityComponent<NIKE::Render::Texture>(tile_entity, NIKE::Render::Texture(texture_name,
			{ 1.0f, 1.0f, 1.0f, 1.0f }, false, 0.5f, false, { 1, 1 }, { 0, 0 }, { flip, false }));
	}
	*/
}