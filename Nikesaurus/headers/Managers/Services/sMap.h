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

#define DEFAULT_MAP_SIZE Vector2i{1,1}
#define DEFAULT_CELL_SIZE Vector2f{256.f,256.f}

namespace NIKE {
	namespace Map {
		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		struct Cell {
			bool b_blocked;
			Vector2f position;

			Cell() : b_blocked{ false } {}
		};

		struct PathNode {
			bool obstacle;
			bool checked;
			float dist_player;
			float dist_enemy;
			Vector2f index;
			std::vector<PathNode*> neighbours;
			PathNode* parent;

			//Constructor
			PathNode() : obstacle{ false }, checked{ false }, dist_player{ static_cast<float>(INFINITY) },
				dist_enemy{ static_cast<float>(INFINITY) }, index{ 0, 0 }, parent{ nullptr } {}


			//Destructor
			~PathNode() {
				neighbours.clear();
			}

			struct PathNodeComparator {
				bool operator()(const PathNode* lhs, const PathNode* rhs) const {
					return (lhs->dist_player + lhs->dist_enemy) > (rhs->dist_player + rhs->dist_enemy);
				}
			};
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

			//// Pathfinding
			std::vector<Vector2f> findPath(Vector2f start, Vector2f goal);
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