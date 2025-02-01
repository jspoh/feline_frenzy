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
		: grid_size{ DEFAULT_GRID_SIZE }, cell_size{ DEFAULT_CELL_SIZE }, cursor_pos{ 0.0f, 0.0f }, b_cell_changed{ false } {

		//Initialize grid
		grid.resize(grid_size.y);
		for (size_t i = 0; i < grid_size.y; ++i) {
			grid.at(i).resize(grid_size.x);
		}

		updateCells();
	}


	void Map::Service::onEvent(std::shared_ptr<Coordinator::EntitiesChanged> event) {

		//Update entity paths
		for (auto it = paths.begin(); it != paths.end();) {
			if (event->entities.find(it->first) == event->entities.end()) {
				it = paths.erase(it);
			}
			else {
				++it;
			}
		}
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

	void Map::Service::gridUpdate() {

		//Reset cell changed
		b_cell_changed = false;

		//Iterate through grid
		for (auto& row : grid) {
			for (auto& cell : row) {
				if (cell.b_blocked_prev != cell.b_blocked) {
					cell.b_blocked_prev = cell.b_blocked;
					b_cell_changed = true;
				}
			}
		}
	}

	bool Map::Service::checkGridChanged() const {
		return b_cell_changed;
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

	/************************
	* AH LIM VERSION
	***********************/
	/*std::vector<Map::Cell> Map::Service::findPath(Vector2i const& start, Vector2i const& goal, bool b_diagonal) {
		const int direction_x[] = { -1, 0, 1, 0 };
		const int direction_y[] = { 0, 1, 0, -1 };
		const int total_directions = 4;

		std::priority_queue<Cell, std::vector<Cell>, std::greater<Cell>> open_list;
		std::unordered_set<Vector2i, Vector2iHasher> closed_list;

		// Initialize cells
		for (int y = 0; y < grid_size.y; ++y) {
			for (int x = 0; x < grid_size.x; ++x) {
				grid[y][x].g = INT_MAX;
				grid[y][x].h = INT_MAX;
				grid[y][x].f = INT_MAX;
				grid[y][x].parent = { -1, -1 };
			}
		}

		// Add blocked cells to the closed list
		for (int y = 0; y < grid_size.y; ++y) {
			for (int x = 0; x < grid_size.x; ++x) {
				if (isCellBlocked(x, y)) {
					closed_list.insert({ x, y });
				}
			}
		}


		// Initialize start cell
		Cell& start_cell = grid[start.y][start.x];
		// Initialize start cell
		start_cell.g = 0;
		start_cell.h = abs(start.x - goal.x) + abs(start.y - goal.y);
		start_cell.f = start_cell.g + start_cell.h;
		start_cell.parent = start;
		open_list.push(start_cell);

		// Start node
		open_list.push(start_cell);

		while (!open_list.empty()) {
			// Get cell with lowest cost from open_list
			Cell current = open_list.top();
			open_list.pop();

			// Mark the current cell as closed
			closed_list.insert(current.index);

			if (current.index == goal) {
				std::vector<Cell> path;
				Vector2i trace = current.index;
				while (trace != start) {
					path.push_back(grid[trace.y][trace.x]);
					trace = grid[trace.y][trace.x].parent;
				}
				//path.push_back(grid[start.y][start.x]);
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


				Cell& neighbor = grid[new_y][new_x];
				int new_g = current.g + 1;
				if (new_g < neighbor.g) {
					neighbor.g = new_g;
					neighbor.h = abs(new_x - goal.x) + abs(new_y - goal.y);
					neighbor.f = neighbor.g + neighbor.h;
					neighbor.parent = current.index;

					// Only push to the open list if not already in it
					if (closed_list.count(neighbor.index) == 0) {
						open_list.push(neighbor);
					}

				}
			}
		}

			// Return empty path when no path found
			return {};
	}*/

	void Map::Service::findPath(Entity::Type entity, const Vector2i& start, const Vector2i& goal, bool b_diagonal) {

		//Custom comparator
		struct CostComparator {
			bool operator()(Cell const* lhs, Cell const* rhs) const {

				//Comparison For Set !Set will ignore elements that are equal hence comparing memory address at the end
				if(*lhs < *rhs) return true;
				if(*lhs > *rhs) return false;
 				return lhs < rhs;
			}
		};

		//Distance to start
		auto g_calculator = [](Cell const& cell) -> int {
			int diagonal{ 14 };
			int updown{ 10 };

			if (std::abs(static_cast<int>(cell.parent->index.x - cell.index.x)) && std::abs(static_cast<int>(cell.parent->index.y - cell.index.y))) {
				return cell.parent->g + diagonal;
			}
			else {
				return cell.parent->g + updown;
			}
			};

		//Distance to goal
		auto h_calculator = [&, b_diagonal](Vector2i const& index, Vector2i const& end) -> int {
			int diagonal{ 14 };
			int updown{ 10 };

			int x_moves{ std::abs(static_cast<int>(end.x - index.x)) };
			int y_moves{ std::abs(static_cast<int>(end.y - index.y)) };

			int diagonal_dist;
			int updown_dist;

			if (b_diagonal) {
				diagonal_dist = Utility::getMin(x_moves, y_moves) * diagonal;
				updown_dist = (Utility::getMax(x_moves, y_moves) - Utility::getMin(x_moves, y_moves)) * updown;
			}
			else {
				diagonal_dist = x_moves * updown;
				updown_dist = y_moves * updown;
			}

			return diagonal_dist + updown_dist;
			};

		//Open list
		std::set<Cell*, CostComparator> open_list;

		//Close list
		std::set<Cell*, CostComparator> closed_list;

		//Calculate h for all cells
		for (size_t i{ 0 }; i < grid.size(); i++) {
			for (size_t j{ 0 }; j < grid.at(i).size(); j++) {
				grid.at(i).at(j).h = h_calculator(Vector2i(static_cast<int>(j), static_cast<int>(i)), goal);
				grid.at(i).at(j).g = 0;
				grid.at(i).at(j).f = 0;
				grid.at(i).at(j).parent = nullptr;
			}
		}

		//Initialize start cell
		Cell* head = &grid.at(start.y).at(start.x);
		head->parent = head;
		open_list.insert(head);
		bool b_path_found = false;

		//Search for path
		while (!open_list.empty()) {

			//Check if goal has been reached
			if (head->index == goal) {
				b_path_found = true;
				break;
			}

			//Add Neighbours Of Nodes Into Open Container
			for (int i = head->index.y - 1; i <= head->index.y + 1; i++) {
				for (int j = head->index.x - 1; j <= head->index.x + 1; j++) {

					//Skip diagonal search
					if (!b_diagonal && ((i == head->index.y - 1 && j == head->index.x - 1) || (i == head->index.y - 1 && j == head->index.x + 1)
						|| (i == head->index.y + 1 && j == head->index.x - 1) || (i == head->index.y + 1 && j == head->index.x + 1))) continue;

					//Skip boundaries, blocked cells and node itself
					if ((i == head->index.y && j == head->index.x) ||
						i >= grid.size() ||
						j >= grid.at(0).size() ||
						i < 0 || j < 0 ||
						grid.at(i).at(j).b_blocked) continue;

					//Check For Duplicates
					if (open_list.find(&grid.at(i).at(j)) != open_list.end() || closed_list.find(&grid.at(i).at(j)) != closed_list.end()) {
						if (g_calculator(grid.at(i).at(j)) < grid.at(i).at(j).g) {
							open_list.erase(open_list.find(&grid.at(i).at(j)));
							grid.at(i).at(j).parent = head;
							grid.at(i).at(j).g = g_calculator(grid.at(i).at(j));
							open_list.insert(&grid.at(i).at(j));
						}
					}
					else {
						grid.at(i).at(j).parent = head;
						grid.at(i).at(j).g = g_calculator(grid.at(i).at(j));
						open_list.insert(&grid.at(i).at(j));
					}
				}
			}

			//Remove head from open list & add to closed list
			closed_list.insert(head);
			open_list.erase(open_list.find(head));
			if (!open_list.empty()) {
				head = *open_list.begin();
			}
		}

		//Find next best path
		if (!b_path_found) {
			if (!closed_list.empty()) {
				head = *closed_list.begin();
			}
		}

		//Trace back path
		std::deque<Cell> path;
		while (head->index != start) {
			path.push_front(grid.at(head->index.y).at(head->index.x));
			head = grid.at(head->index.y).at(head->index.x).parent;
		}

		//Update entity path
		paths[entity].path = path;
		paths[entity].b_finished = false;
		paths[entity].goal = grid.at(goal.y).at(goal.x);
		if (!paths[entity].path.empty()) {
			paths[entity].end = paths[entity].path.back();
		}
	}

	Map::Path Map::Service::getPath(Entity::Type entity) {
		if (paths.find(entity) != paths.end()) {
			return paths.at(entity);
		}

		// If not path found, return empty path obj
		throw Path();
	}

	bool Map::Service::checkPath(Entity::Type entity) const {

		return paths.find(entity) != paths.end();
	}

	bool Map::Cell::operator<(Map::Cell const& other) const {
		return (f != other.f) ? (f < other.f) : (h != other.h) ? h < other.h : g < other.g;
	}

	bool Map::Cell::operator>(Map::Cell const& other) const {
		return (f != other.f) ? (f > other.f) : (h != other.h) ? h > other.h : g > other.g;
	}
}

