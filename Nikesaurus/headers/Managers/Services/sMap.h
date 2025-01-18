/*****************************************************************//**
 * \file   sMap.h
 * \brief  Map manager
 *
 * \author Bryan Soh, 2301238, z.soh@digipen.edu (100%)
 * \date   November 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#include "Core/Engine.h"
#include "Managers/ECS/mCoordinator.h"

#ifndef M_MAP_HPP
#define M_MAP_HPP

#define DEFAULT_GRID_SIZE Vector2i{1,1}
#define DEFAULT_CELL_SIZE Vector2f{256.f,256.f}

namespace NIKE {
	namespace Map {
		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		struct Cell {
			bool b_blocked;
			Vector2f position;
			Vector2i index;

			Vector2i parent;

			// Values used by the A* algorithm
			/////////////////////////////////////////////////////////////////////////////////
			// G(cost from start) is the cost to reach a node from the start node
			// H(Heuristic cost to goal) is an estimate cost from current node to the goal
			// F(Total Cost) is the value used to prio nodes in prio queue
			/////////////////////////////////////////////////////////////////////////////////
			int f, g, h;

			Cell() = default;

			Cell(int _x, int _y) : b_blocked{ false }, position{}, index{_x, _y}, parent{ -1, -1 }, f{0}, g{0}, h{0}
			{
			}

			// Overload comparison operators for priority queue
			bool operator>(const Cell& other) const;
			bool operator==(const Cell& other) const;
		};

		class NIKE_API Service : public Events::IEventListener<Input::MouseMovedEvent> {
		public:
			Service();
			~Service() = default;

			//Init
			void init(Vector2i const& gridsize, Vector2f const& cellsize);

			//Changes whether cell is blocked
			void setCellBlocked(size_t x, size_t y, bool blocked);

			//Check if cell is blocked
			bool isCellBlocked(size_t x, size_t y) const;

			//Set grid size
			void setGridSize(Vector2i const& size);

			//Get grid size
			Vector2i getGridSize() const;

			//Set cell size
			void setCellSize(Vector2f const& size);

			//Get cell size
			Vector2f getCellSize() const;

			//Get grid scale
			Vector2f getGridScale() const;

			// Reset cell size and grid size
			void resetGrid();

			//Get cell cursor is in
			std::optional<std::reference_wrapper<Map::Cell>> getCursorCell();

			//Get cell based on position
			std::optional<std::reference_wrapper<Map::Cell>> getCellAtPosition(Vector2f const& position);

			//Get grid
			std::vector<std::vector<Cell>>const& getGrid() const;

			//Serialize map
			nlohmann::json serialize() const;

			//Deserialize map
			void deserialize(nlohmann::json const& data);

			// Pathfinding
			std::vector<Cell> findPath(Cell const& start, Cell const& goal);

			// Debug purposes
			void PrintPath(const std::vector<Cell>& path);

			//void resetPathfindComp(Pathfinding::Path& path);
		private:

			//Internal cell pos update
			void updateCells();

			//On mouse move event
			void onEvent(std::shared_ptr<Input::MouseMovedEvent> event) override;

			//Grid vector
			std::vector<std::vector<Cell>> grid;

			//Grid size
			Vector2i grid_size;

			//Cell size
			Vector2f cell_size;

			//Grid scale
			Vector2f grid_scale;

			//Cursor position relative to game window
			Vector2f cursor_pos;
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}
#endif // !M_MAP_HPP