/*****************************************************************//**
 * \file   sMap.h
 * \brief  Map system header
 *
 * \author Bryan Soh, 2301238, z.soh@digipen.edu (100%)
 * \date   November 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
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
			bool b_blocked_prev;
			Vector2f position;
			Vector2i index;

			// Ah ho uses Cell* as parent
			// Ah lim uses Vectori as parent
			//Vector2i parent;

			Cell* parent = nullptr;

			// Values used by the A* algorithm
			/////////////////////////////////////////////////////////////////////////////////
			// G(cost from start) is the cost to reach a node from the start node
			// H(Heuristic cost to goal) is an estimate cost from current node to the goal
			// F(Total Cost) is the value used to prio nodes in prio queue
			/////////////////////////////////////////////////////////////////////////////////
			int f, g, h;

			Cell(int _x, int _y) : b_blocked{ false }, b_blocked_prev{ false }, position {}, index{ _x, _y }, f{ 0 }, g{ 0 }, h{ 0 }
			{
			}

			Cell(Vector2i input) : b_blocked{ false }, b_blocked_prev{ false }, position{}, index{ input.x, input.y }, f{ 0 }, g{ 0 }, h{ 0 }
			{
			}

			Cell() : b_blocked{ false }, b_blocked_prev{ false }, position{}, index{}, f{ 0 }, g{ 0 }, h{ 0 }
			{
			}

			//Lesser Than Comparison
			bool operator<(Cell const& other) const;

			//Greater Than Comparison
			bool operator>(Cell const& other) const;
		};

		//Path data object
		struct Path {
			std::deque<Cell> path;
			Cell end;
			Cell goal;
			bool b_finished;
		};

		class NIKE_API Service 
			:	public Events::IEventListener<Input::MouseMovedEvent>,
				public Events::IEventListener<Coordinator::EntitiesChanged>
		{
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

			//Get cell index based on world cords
			std::optional<Vector2i> getCellIndexFromCords(Vector2f const& position);

			//Get grid
			std::vector<std::vector<Cell>>const& getGrid() const;

			//Cell state checking
			void gridUpdate();

			//Check grid changed
			bool checkGridChanged() const;

			//Serialize map
			nlohmann::json serialize() const;

			//Deserialize map
			void deserialize(nlohmann::json const& data);

			//Pathfinding
			void findPath(Entity::Type entity, Vector2i const& start, Vector2i const& goal, bool b_diagonal = false);

			//Get entity path
			Path getPath(Entity::Type entity);

			//Check entity path
			bool checkPath(Entity::Type entity) const;
		private:

			//On entities changed event
			void onEvent(std::shared_ptr<Coordinator::EntitiesChanged> event) override;

			//Internal cell pos update
			void updateCells();

			//On mouse move event
			void onEvent(std::shared_ptr<Input::MouseMovedEvent> event) override;

			//Pathfinding paths
			std::unordered_map<Entity::Type, Path> paths;

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

			//Boolean to signal a change in cell states within grid
			bool b_cell_changed;
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}
#endif // !M_MAP_HPP