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

namespace NIKE {
	namespace Map {
		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		class NIKE_API Service {
		public:
			Service() : width(0), height(0), blockedCells{} {}
			~Service() = default;

			// Initializa Grid
			void initializeGrid(float map_width, float map_height, const std::vector<std::vector<int>>& tileData);

			// Check if cell is blocked
			bool isCellBlocked(float x, float y) const;

			// Changes whether cell is blocked
			void setCellBlocked(float x, float y, bool blocked);

			// Pathfinding
			std::vector<NIKE::Math::Vector2f> findPath(NIKE::Math::Vector2f start, NIKE::Math::Vector2f goal);

			//Load map from file path
			void loadMapFromFile(const std::string& file,
				std::shared_ptr<NIKE::Scenes::Layer>& background_layer,
				std::shared_ptr<NIKE::Scenes::Layer>& player_layer,
				std::vector<std::vector<int>>& grid,
				const NIKE::Math::Vector2<float>& center);

		private:
			float width, height;
			std::vector<bool> blockedCells;  // Example structure for grid data
			//Create Tile
			void createTile(int tileID, int row, int col, float tile_size, float offset_x, float offset_y, int width, int height,
				std::shared_ptr<NIKE::Scenes::Layer>& background_layer,
				std::shared_ptr<NIKE::Scenes::Layer>& player_layer);
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}
#endif // !M_MAP_HPP