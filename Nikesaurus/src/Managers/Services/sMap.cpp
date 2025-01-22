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
		float top = (grid_scale.y / 2.0f);
		for (int i = 0; i < grid_size.y; ++i) {
			float left = -(grid_scale.x / 2.0f);
			for (int j = 0; j < grid_size.x; ++j) {
				grid.at(i).at(j).position = { left + (cell_size.x / 2.0f), top - (cell_size.y / 2.0f) };

				grid.at(i).at(j).index = { j, i };

				//if (getCellIndexFromCords(grid.at(i).at(j).position).has_value())
				//{
				//	grid.at(i).at(j).index = getCellIndexFromCords(grid.at(i).at(j).position).value();
				//}
				left += cell_size.x;
			}
			top -= cell_size.y;
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

		Vector2f translated_cursor { cursor_pos.x + (grid_scale.x / 2.0f), -cursor_pos.y + (grid_scale.y / 2.0f) };

		//Get index for cell
		Vector2i cell_index{ static_cast<int>(translated_cursor.x / cell_size.x), static_cast<int>(translated_cursor.y / cell_size.y) };

		//Check if index is valid & if translated cursor is positive
		if (translated_cursor.x < 0 || translated_cursor.y < 0 || cell_index.x < 0 || cell_index.x >= grid_size.x || cell_index.y < 0 || cell_index.y >= grid_size.y) {
			return std::nullopt;
		}
		else {
			return grid.at(cell_index.y).at(cell_index.x);
		}
	}

	std::optional<std::reference_wrapper<Map::Cell>> Map::Service::getCellAtPosition(Vector2f const& position) {

		//Translate position
		Vector2f translated_pos{ position.x + (grid_scale.x / 2.0f), -position.y + (grid_scale.y / 2.0f) };

		//Get index for cell
		Vector2i cell_index{ static_cast<int>(translated_pos.x / cell_size.x), static_cast<int>(translated_pos.y / cell_size.y) };

		//Check if index is valid
		if (cell_index.x < 0 || cell_index.x >= grid_size.x || cell_index.y < 0 || cell_index.y >= grid_size.y) {
			return std::nullopt;
		}
		else {
			return grid.at(cell_index.y).at(cell_index.x);
		}
	}

	std::optional<Vector2i> Map::Service::getCellIndexFromCords(Vector2f const& position)
	{
		//Translate position
		Vector2f translated_pos{ position.x + (grid_scale.x / 2.0f), -position.y + (grid_scale.y / 2.0f) };

		//Get index for cell
		Vector2i cell_index{ static_cast<int>(translated_pos.x / cell_size.x), static_cast<int>(translated_pos.y / cell_size.y) };

		//Check if index is valid
		if (cell_index.x < 0 || cell_index.x >= grid_size.x || cell_index.y < 0 || cell_index.y >= grid_size.y) {
			return std::nullopt;
		}

		return cell_index;
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
				cell_json = { {"Blocked", cell.b_blocked}, {"Position", cell.position.toJson()}};
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
				Cell cell{};
				// Note 1: Blocked boolean is properly seri and registered. suspect: somewhr might be interferring with the boolean?
				cell.b_blocked = cell_json.at("Blocked").get<bool>();
				cell.position.fromJson(cell_json.at("Position"));
				row.push_back(cell);
			}
			grid.push_back(row);
		}

		updateCells();
	}

	std::vector<Map::Cell> Map::Service::findPath(const Cell& start, const Cell& goal) {
		const int direction_x[] = { -1, 0, 1, 0 };
		const int direction_y[] = { 0, 1, 0, -1 };
		const int total_directions = 4;

		std::priority_queue<Cell, std::vector<Cell>, std::greater<Cell>> open_list;
		std::unordered_set<Vector2i, Vector2iHasher> closed_list;

		// Add blocked cells to the closed list
		for (int y = 0; y < grid_size.y; ++y) {
			for (int x = 0; x < grid_size.x; ++x) {
				if (isCellBlocked(x, y)) {
					closed_list.insert({ x, y });
				}
			}
		}

		// Lim's debug
		//if (!debug)
		//{
		//	for (auto& elems : closed_list)
		//	{
		//		cout << elems.y << ", " << elems.x << endl;
		//	}
		//	debug = true;
		//}


		// Initialize start cell
		Cell start_cell = start;
		start_cell.g = 0;
		start_cell.h = abs(start.index.x - goal.index.x) + abs(start.index.y - goal.index.y);
		start_cell.f = start_cell.g + start_cell.h;
		start_cell.parent = start.index;
		open_list.push(start_cell);

		while (!open_list.empty()) {
			// Get cell with lowest cost from open_list
			Cell current = open_list.top();
			open_list.pop();

			// Move the current cell to the closed list
			closed_list.insert(current.index);

			if (current.index == goal.index) {
				std::vector<Cell> path;
				Vector2i trace = goal.index;
				while (trace != start.index) {
					path.push_back(grid[trace.y][trace.x]);
					trace = grid[trace.y][trace.x].parent;
				}
				path.push_back(start);
				std::reverse(path.begin(), path.end());
				return path;
			}

			// Check neighbors
			for (int i = 0; i < total_directions; ++i) {
				int new_x = current.index.x + direction_x[i];
				int new_y = current.index.y + direction_y[i];
				Vector2i neighbor_index = { new_x, new_y };

				// Skip if neighbor is out of bounds, blocked, or already processed
				if (new_x < 0 || new_x >= grid_size.x ||
					new_y < 0 || new_y >= grid_size.y ||
					isCellBlocked(new_x, new_y) ||
					closed_list.count(neighbor_index)) {
					continue;
				}

				std::unordered_set<Vector2i, Vector2iHasher> in_open_list;

				Cell& neighbor = grid[new_y][new_x];
				int new_g = current.g + 1;

				if (neighbor.parent.x == -1 && neighbor.parent.y == -1 || new_g < neighbor.g) {
					neighbor.g = new_g;
					neighbor.h = abs(new_x - goal.index.x) + abs(new_y - goal.index.y);
					neighbor.f = neighbor.g + neighbor.h;
					neighbor.parent = current.index;

					// Only push to the open list if not already in it
					if (!in_open_list.count(neighbor.index)) {
						open_list.push(neighbor);
						in_open_list.insert(neighbor.index);
					}
				}

			}
		}

		// If no path found, return shortest path exists
		std::vector<Cell> shortest_path;

		// Default max value
		int shortest_f = INT_MAX;

		// Check for shortest path from closed list
		for (const auto& cell : closed_list) {
			Cell& current = grid[cell.y][cell.x];
			if (current.f < shortest_f) {
				shortest_f = current.f;
				shortest_path = { current };
			}
		}

		// Trace back the shortest path if available
		if (!shortest_path.empty()) {
			Vector2i trace = shortest_path.front().index;
			while (trace != start.index) {
				if (trace.x >= 0 && trace.x < grid_size.x && trace.y >= 0 && trace.y < grid_size.y) {
					shortest_path.push_back(grid[trace.y][trace.x]);
					trace = grid[trace.y][trace.x].parent;
				}
				else {
					// Avoid out-of-bounds access (if not will be thrown)
					break; 
				}
			}
			std::reverse(shortest_path.begin(), shortest_path.end());
		}

		return shortest_path;
		
	}


	bool Map::Cell::operator>(const Cell& other) const
	{
		return f > other.f;
	}

	bool Map::Cell::operator==(const Cell& other) const
	{
		return index.x == other.index.x && index.y == other.index.y;
	}

	// TO BE DELETED
	void Map::Service::PrintPath(const std::vector<Cell>& path)
	{
		for (const Cell& cell : path)
		{
			cout << "( " << cell.index.y << ", " << cell.index.x << " )";
		}
		cout << endl;
	}

	//void Map::Service::resetPathfindComp(Pathfinding::Path& path)
	//{
	//	path.path.clear();
	//	path.path_found = false;
	//	path.start_cell = Cell{};
	//	path.goal_cell = Cell{};
	//}

}

