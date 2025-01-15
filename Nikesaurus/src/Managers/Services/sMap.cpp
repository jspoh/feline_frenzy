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
		Vector2i index{ static_cast<int>(translated_cursor.x / cell_size.x), static_cast<int>(translated_cursor.y / cell_size.y) };

		cout << index.x << ", " << index.y << endl;

		//Check if index is valid
		if (index.x < 0 || index.x >= grid_size.x || index.y < 0 || index.y >= grid_size.y) {
			return std::nullopt;
		}
		else {
			return grid.at(index.y).at(index.x);
		}
	}

	std::optional<std::reference_wrapper<Map::Cell>> Map::Service::getCellAtPosition(Vector2f const& position) {

		//Translate position
		Vector2f translated_pos{ position.x + (grid_scale.x / 2.0f), position.y + (grid_scale.y / 2.0f) };

		//Get index for cell
		Vector2i index{ static_cast<int>(translated_pos.x / cell_size.x), static_cast<int>(translated_pos.y / cell_size.y) };

		//Check if index is valid
		if (index.x < 0 || index.x >= grid_size.x || index.y < 0 || index.y >= grid_size.y) {
			return std::nullopt;
		}
		else {
			return grid.at(index.y).at(index.x);
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

	std::vector<Vector2f> Map::Service::findPath(Vector2f start, Vector2f goal) {
		std::map<Vector2i, PathNode> node_map;

		// Initialize nodes
		for (int y = 0; y < getGridSize().y; ++y) {
			for (int x = 0; x < getGridSize().x; ++x) {
				PathNode node;
				node.index = Vector2i(x, y);
				node.obstacle = grid[y][x].b_blocked;
				node_map[{x, y}] = node;
			}
		}

		// Set neighbors
		for (auto& elem : node_map) {
			int x = elem.first.x, y = elem.first.y;
			std::vector<Vector2i> neighbors = { {x - 1, y}, {x + 1, y}, {x, y - 1}, {x, y + 1} };
			for (const auto& neighbor : neighbors) {
				if (neighbor.x >= 0 && neighbor.x < grid_size.x &&
					neighbor.y >= 0 && neighbor.y < grid_size.y &&
					!node_map[neighbor].obstacle) {
					elem.second.neighbours.push_back(&node_map[neighbor]);
				}
			}
		}

		// A* Algorithm
		//Translate position
		Vector2f translated_start{ start.x + (grid_scale.x / 2.0f), start.y + (grid_scale.y / 2.0f) };
		Vector2f translated_goal{ goal.x + (grid_scale.x / 2.0f), goal.y + (grid_scale.y / 2.0f) };

		//Get index for start and goal
		Vector2i start_index{ static_cast<int>(translated_start.x / cell_size.x), static_cast<int>(translated_start.y / cell_size.y) };
		Vector2i goal_index{ static_cast<int>(translated_goal.x / cell_size.x), static_cast<int>(translated_goal.y / cell_size.y) };

		PathNode* start_node = &node_map[start_index];
		PathNode* goal_node = &node_map[goal_index];

		std::priority_queue<PathNode*, std::vector<PathNode*>, NIKE::Map::PathNode::PathNodeComparator> open_list;
		start_node->dist_player = 0;
		start_node->dist_enemy = abs(goal.x - start.x) + abs(goal.y - start.y);
		open_list.push(start_node);

		while (!open_list.empty()) {
			PathNode* current = open_list.top();
			open_list.pop();

			if (current == goal_node) break;
			current->checked = true;

			for (auto neighbor : current->neighbours) {
				if (neighbor->checked || neighbor->obstacle) continue;

				float newDist = current->dist_player + 1; 
				if (newDist < neighbor->dist_player) {
					neighbor->dist_player = newDist;
					neighbor->dist_enemy = abs(goal.x - neighbor->index.x) + abs(goal.y - neighbor->index.y);
					neighbor->parent = current;
					open_list.push(neighbor);
				}
			}
		}

		// Construct the path
		std::vector<Vector2f> path;
		PathNode* current_node = goal_node;
		while (current_node && current_node != start_node) {
			path.push_back(Vector2f(current_node->index.x, current_node->index.y));
			current_node = current_node->parent;
		}

		if (current_node == start_node) path.push_back(Vector2f(start_node->index.x, start_node->index.y));
		else {
			return {};
		}

		std::reverse(path.begin(), path.end());

		return path;
	}

}	