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

        // Register Pathfinding component for serialization
        NIKE_SERIALIZE_SERVICE->registerComponent<Pathfinding::Path>(
            // Serialization
            [](Pathfinding::Path const& comp) -> nlohmann::json {
                nlohmann::json path_json;
                for (auto const& point : comp.path) {
                    path_json.push_back(point.index.toJson());
                }
                return {
                    { "Path", path_json },
                    { "StartCell", {
                        { "Index", { comp.start_cell.index.x, comp.start_cell.index.y } },
                        { "Blocked", comp.start_cell.b_blocked },
                        { "Position", comp.start_cell.position.toJson() }
                    }},
                    { "GoalCell", {
                        { "Index", { comp.goal_cell.index.x, comp.goal_cell.index.y } },
                        { "Blocked", comp.goal_cell.b_blocked },
                        { "Position", comp.goal_cell.position.toJson() }
                    }},
                    { "PathFinished", comp.b_finished }
                };
            },

            // Deserialization
            [](Pathfinding::Path& comp, nlohmann::json const& data) {
                // Deserialize the path
                comp.path.clear();
                for (auto const& point_data : data.at("Path")) {
                    Map::Cell point;
                    point.index.fromJson(point_data);
                    comp.path.push_back(point);
                }

                // Deserialize start cell
                auto start_data = data.at("StartCell");
                comp.start_cell.index = { start_data.at("Index")[0].get<int>(), start_data.at("Index")[1].get<int>() };
                comp.start_cell.b_blocked = start_data.at("Blocked").get<bool>();
                comp.start_cell.position.fromJson(start_data.at("Position"));

                // Deserialize goal cell
                auto goal_data = data.at("GoalCell");
                comp.goal_cell.index = { goal_data.at("Index")[0].get<int>(), goal_data.at("Index")[1].get<int>() };
                comp.goal_cell.b_blocked = goal_data.at("Blocked").get<bool>();
                comp.goal_cell.position.fromJson(goal_data.at("Position"));

                // Deserialize path_found
                comp.b_finished = data.at("PathFinished").get<bool>();
            }
        );
    }

}