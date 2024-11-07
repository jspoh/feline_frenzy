/*****************************************************************//**
 * \file   cPathfinding.cpp
 * \brief  Pathfinding components
 *
 * \author Bryan Soh, 2301238, z.soh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cPathfinding.h"

namespace NIKE {
    void Pathfinding::registerComponents() {
        // Register the Pathfinding component with the ECS manager
        NIKE_ECS_MANAGER->registerComponent<Pathfinding::Path>();

        // Register Pathfinding for serialization
        NIKE_SERIALIZE_SERVICE->registerComponent<Pathfinding::Path>(
            // Serialization
            [](Pathfinding::Path const& comp) -> nlohmann::json {
                nlohmann::json path_json;
                for (auto const& point : comp.path) {
                    path_json.push_back(point.toJson()); // Assuming toJson() is defined for Vector2f
                }
                return {
                    { "Path", path_json },
                    { "CurrentIndex", comp.currentIndex },
                    { "PathFound", comp.pathFound }
                };
            },

            // Deserialization
            [](Pathfinding::Path& comp, nlohmann::json const& data) {
                comp.path.clear();
                for (auto const& point_data : data.at("Path")) {
                    NIKE::Math::Vector2f point;
                    point.fromJson(point_data); // Assuming fromJson() is defined for Vector2f
                    comp.path.push_back(point);
                }
                comp.currentIndex = data.at("CurrentIndex").get<int>();
                comp.pathFound = data.at("PathFound").get<bool>();
            }
        );
    }
}