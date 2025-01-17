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
				Cell cell{};
				cell.b_blocked = cell_json.at("Blocked").get<bool>();
				cell.position.fromJson(cell_json.at("Position"));
				row.push_back(cell);
			}
			grid.push_back(row);
		}

		updateCells();
	}

	std::vector<NIKE::Map::Cell> Map::Service::findPath(const Cell& start, const Cell& goal) {
		// Init directions, possible directions: left, right, up down
		const int direction_x[] = { -1, 0, 1, 0 };
		const int direction_y[] = { 0, 1, 0, -1 };

		// Change here when have more directions
		int total_directions{ 4 };

		// Initialize the open and closed lists
		std::priority_queue <Cell, std::vector<Cell>, std::greater<Cell>> open_list;
		std::vector<std::vector<bool>> closed_list(grid.size(), std::vector<bool>(grid[0].size(), false));
		
		// Start Node
		open_list.push(start);

		while (!open_list.empty()) {
			// Get cell with lowest f value
			Cell current = open_list.top();
			open_list.pop();

			// Check if current is the goal node
			if (current == goal)
			{
				// Reconstruct the path
				std::vector<Cell> path;
				if (!(current == goal))
				{
					path.push_back(current);
					current = grid[current.index.x][current.index.y];
				}
				path.push_back(start);
				std::reverse(path.begin(), path.end());
				return path;
			}

			// Mark the current cell as closed
			closed_list[current.index.x][current.index.y] = true;

			for (size_t i{0}; i < total_directions; ++i)
			{
				int new_x = current.index.x + direction_x[i];
				int new_y = current.index.y + direction_y[i];

				// Check if neighbour valid
				if (new_x >= 0 && new_x < grid.size() && new_y >= 0 && new_y < grid[0].size())
				{
					// Check if cell blocked
					if (!grid[new_x][new_y].b_blocked && !closed_list[new_x][new_y])
					{
						Cell neighbor(new_x, new_y);
						int new_g = current.g + 1;

						// Check if the neighbor is not in the open list or has a lower g value
						if (new_g < neighbor.g || !closed_list[new_x][new_y]) {
							neighbor.g = new_g;
							neighbor.h = abs(new_x - goal.index.x) + abs(new_y - goal.index.y);
							neighbor.f = neighbor.g + neighbor.h;
							// Update the parent of the neighbor
							grid[new_x][new_y] = current;
							// Add the neighbor to the open list
							open_list.push(neighbor); 
						}
					}
				}
			}

		}

		// No path found
		return std::vector<Cell>();
	}

	bool Map::Cell::operator>(const Cell& other) const
	{
		return f > other.f;
	}

	bool Map::Cell::operator==(const Cell& other) const
	{
		return position.x == other.position.x && position.y == other.position.y;
	}

	// TO BE DELETED
	void Map::Service::PrintPath(const std::vector<Cell>& path)
	{
		for (const Cell& cell : path)
		{
			cout << "(" << cell.index.x << ", " << cell.index.y << ") ";
		}
		cout << endl;
	}

}

