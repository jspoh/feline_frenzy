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

	Map::Service::Service() 
		: grid_size{ DEFAULT_GRID_SIZE }, cell_size{ DEFAULT_CELL_SIZE }, cursor_pos{ 0.0f, 0.0f } {

		//Initialize grid
		grid.resize(grid_size.y);
		for (size_t i = 0; i < grid_size.y; ++i) {
			grid.at(i).resize(grid_size.x);
		}

		updateCells();
	}

	void Map::Service::updateCells() {

		grid_scale = { grid_size.x * cell_size.x, grid_size.y * cell_size.y };

		//Update cells with correct position
		float top = -(grid_scale.y / 2.0f);
		for (size_t i = 0; i < grid_size.y; ++i) {
			float left = -(grid_scale.x / 2.0f);
			for (size_t j = 0; j < grid_size.x; ++j) {
				grid.at(i).at(j).position = { left + (cell_size.x / 2.0f), top + (cell_size.y / 2.0f) };
				left += cell_size.x;
			}
			top += cell_size.y;
		}
	}

	void Map::Service::onEvent(std::shared_ptr<Input::MouseMovedEvent> event) {
		cursor_pos = event->world_pos;
	}

	void Map::Service::init(Vector2i const& gridsize, Vector2f const& cellsize) {
		grid_size = gridsize;
		cell_size = cellsize;

		//Initialize grid
		grid.resize(grid_size.y);
		for (size_t i = 0; i < grid_size.y; ++i) {
			grid.at(i).resize(grid_size.x);
		}

		updateCells();
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
		for (size_t i = 0; i < grid_size.y; ++i) {
			grid.at(i).resize(grid_size.x);
		}

		updateCells();
	}

	Vector2i Map::Service::getGridSize() const {
		return grid_size;
	}

	void Map::Service::setCellSize(Vector2f const& size) {
		cell_size = size;

		updateCells();
	}

	Vector2f Map::Service::getCellSize() const {
		return cell_size;
	}

	Vector2f Map::Service::getGridScale() const {
		return grid_scale;
	}

	void Map::Service::resetGrid()
	{
		cell_size = { DEFAULT_CELL_SIZE };
		grid_size = { DEFAULT_GRID_SIZE };
	}

	std::optional<std::reference_wrapper<Map::Cell>> Map::Service::getCursorCell() {

		Vector2f translated_cursor { cursor_pos.x + (grid_scale.x / 2.0f), cursor_pos.y + (grid_scale.y / 2.0f) };

		//Get index for cell
		Vector2i cell_index{ static_cast<int>(translated_cursor.x / cell_size.x), static_cast<int>(translated_cursor.y / cell_size.y) };

		//Check if index is valid
		if (cell_index.x < 0 || cell_index.x >= grid_size.x || cell_index.y < 0 || cell_index.y >= grid_size.y) {
			return std::nullopt;
		}
		else {
			grid.at(cell_index.y).at(cell_index.x).index = cell_index;
			return grid.at(cell_index.y).at(cell_index.x);
		}
	}

	std::optional<std::reference_wrapper<Map::Cell>> Map::Service::getCellAtPosition(Vector2f const& position) {

		//Translate position
		Vector2f translated_pos{ position.x + (grid_scale.x / 2.0f), position.y + (grid_scale.y / 2.0f) };

		//Get index for cell
		Vector2i cell_index{ static_cast<int>(translated_pos.x / cell_size.x), static_cast<int>(translated_pos.y / cell_size.y) };

		//Check if index is valid
		if (cell_index.x < 0 || cell_index.x >= grid_size.x || cell_index.y < 0 || cell_index.y >= grid_size.y) {
			return std::nullopt;
		}
		else {
			grid.at(cell_index.y).at(cell_index.x).index = cell_index;
			return grid.at(cell_index.y).at(cell_index.x);
		}
	}

	std::vector<std::vector<Map::Cell>>const& Map::Service::getGrid() const {
		return grid;
	}

	nlohmann::json Map::Service::serialize() const {
		nlohmann::json data;

		data =	{
				{"Grid_Size", grid_size.toJson() },
				{"Cell_Size", cell_size.toJson() }
				};

		//Serialize grid
		data["Grid"] = nlohmann::json::array();
		for (const auto& row : grid) {
			nlohmann::json row_json = nlohmann::json::array();
			for (const auto& cell : row) {
				nlohmann::json cell_json;
				cell_json = { {"Blocked", cell.b_blocked}, {"Position", cell.position.toJson()} };
				row_json.push_back(cell_json);
			}
			data["Grid"].push_back(row_json);
		}

		return data;
	}

	void Map::Service::deserialize(nlohmann::json const& data) {
		grid_size.fromJson(data.at("Grid_Size"));
		cell_size.fromJson(data.at("Cell_Size"));

		//Deserialize grid
		grid.clear();
		for (const auto& row_json : data["Grid"]) {
			std::vector<Cell> row;
			for (const auto& cell_json : row_json) {
				Cell cell;
				cell.b_blocked = cell_json.at("Blocked").get<bool>();
				cell.position.fromJson(cell_json.at("Position"));
				row.push_back(cell);
			}
			grid.push_back(row);
		}

		updateCells();
	}

	std::vector<Vector2f> Map::Service::findPath(const Vector2f& start, const Vector2f& goal) {
		auto start_cell = getCellAtPosition(start);
		auto goal_cell = getCellAtPosition(goal);

		if (!start_cell.has_value() || !goal_cell.has_value()) {
			return {};
		}

		PathNode* start_node = &node_map[start_cell.value().get().index];
		PathNode* goal_node = &node_map[goal_cell.value().get().index];
		Vector2i goal_index = goal_cell.value().get().index;
		Vector2i start_index = start_cell.value().get().index;

		// Initialize nodes
		for (int y = 0; y < getGridSize().y; ++y) {
			for (int x = 0; x < getGridSize().x; ++x) {
				PathNode node;
				node.index = Vector2i(x, y);
				node.obstacle = grid[x][y].b_blocked;
				node_map[{x, y}] = node;
			}
		}

		// Set neighbors
		for (auto& elem : node_map) {
			int x = elem.first.x, y = elem.first.y;
			std::vector<Vector2i> neighbors = { {x - 1, y}, {x + 1, y}, {x, y - 1}, {x, y + 1} };
			for (const auto& neighbor : neighbors) {
				if (neighbor.x >= 0 && neighbor.x < getGridSize().x &&
					neighbor.y >= 0 && neighbor.y < getGridSize().y &&
					!node_map[neighbor].obstacle) {
					elem.second.neighbours.push_back(&node_map[neighbor]);
				}
			}
		}

		std::priority_queue<PathNode*, std::vector<PathNode*>, NIKE::Map::PathNode::PathNodeComparator> open_list;
		start_node->dist_player = 0;
		start_node->parent = start_node;
		start_node->dist_enemy = static_cast<float>(abs(goal_index.x - start_index.x) + abs(goal_index.y - start_index.y));
		open_list.push(start_node);

		while (!open_list.empty()) {
			PathNode* current = open_list.top();
			open_list.pop();

			if (current == goal_node) break;
			current->checked = true;

			// Only consider left right up down paths
			std::vector<Vector2i> directions = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
			for (auto& direction : directions) {
				Vector2i neighbor_index = current->index + direction;
				if (isValidCell(neighbor_index) && !node_map[neighbor_index].checked && !node_map[neighbor_index].obstacle) {
					float new_dist = current->dist_player + 1;
					if (new_dist < node_map[neighbor_index].dist_player) {
						node_map[neighbor_index].dist_player = new_dist;
						node_map[neighbor_index].dist_enemy = static_cast<float>(abs(goal_index.x - neighbor_index.x) + abs(goal_index.y - neighbor_index.y));
						node_map[neighbor_index].parent = current;
						open_list.push(&node_map[neighbor_index]);
					}
				}
			}
		}


		// Construct the path
		std::vector<Vector2f> path{};
		PathNode* current_node = goal_node;
		while (current_node) {
			if (current_node->obstacle) break;
			path.push_back(Vector2f(static_cast<float>(current_node->index.x), static_cast<float>(current_node->index.y)));
			if (current_node == start_node) break;

			current_node = current_node->parent;
		}

		std::reverse(path.begin(), path.end());

		return path;
	}

	bool Map::Service::isPlayerSurrounded(Map::PathNode* goal) {
		bool surrounded = true;
		for (Map::PathNode*& neighbor : goal->neighbours) {
			if (!neighbor->obstacle) {
				surrounded = false;
				break;
			}
		}
		return surrounded;
	}


	bool Map::Service::isValidCell(const Vector2i& index) {
		// For checking bounds for x axis
		return index.x >= 0 && index.x < grid_size.x &&
			// For checking bounds for y axis
			index.y >= 0 && index.y < grid_size.y &&
			// Ensure it's not an obstacle
			!node_map[index].obstacle; 
	}


}	