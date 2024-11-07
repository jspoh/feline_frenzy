/*****************************************************************//**
 * \file   sysPathfinding.cpp
 * \brief  Pathfinding System
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (100%)
 * \date   November 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Systems/sysPathfinding.h"


//namespace NIKE {
//	void Pathfinding::Manager::init() {}
//
//	void Pathfinding::Manager::update() {
//        //Iterate through layers
//        for (auto& layer : NIKE_SCENES_SERVICE->getCurrScene()->getLayers()) {
//            //SKip inactive layer
//            if (!layer->getLayerState())
//                continue;
//
//            for (auto& entity : entities) {
//                //Skip entities that are not present within layer & entities without transform component
//                if (layer->getLayerID() != NIKE_ECS_MANAGER->getEntityLayerID(entity) || !NIKE_ECS_MANAGER->checkEntityComponent<Transform::Transform>(entity))
//                    continue;
//
//                //Update entities with pathfinding
//                if (NIKE_ECS_MANAGER->checkEntityComponent<NIKE::Pathfinding::Pathfinding>(entity) &&
//                    NIKE_ECS_MANAGER->checkEntityComponent<Transform::Transform>(entity)) {
//                    auto& pathfinding = NIKE_ECS_SERVICE->getEntityComponent<NIKE::Pathfinding::Pathfinding>(entity);
//
//                    // Add your pathfinding logic here, such as updating the entity's path
//                    if (pathfinding.pathFound) {
//                        moveEntity(entity);
//                    }
//                }
//            }
//        }
//	}
//
//    void Pathfinding::Manager::moveEntity(NIKE::Entity::Type entity) {
//        auto& transform = NIKE_ECS_SERVICE->getEntityComponent<NIKE::Transform::Transform>(entity);
//        NIKE::Math::Vector2f entityPos = transform.position;
//
//        auto& pathfinding = NIKE_ECS_SERVICE->getEntityComponent<NIKE::Pathfinding::Pathfinding>(entity);
//        if (pathfinding.currentIndex < pathfinding.path.size()) {
//            NIKE::Math::Vector2f target = pathfinding.path[pathfinding.currentIndex];
//            NIKE::Math::Vector2f direction = target - entityPos;
//            direction = direction.normalize();
//
//            float dt = NIKE_WINDOWS_SERVICE->getDeltaTime();
//
//            // Move towards the next waypoint
//            transform.position += direction * dt * 100.0f; // Speed factor
//
//            // If the entity has reached the waypoint, update to the next waypoint
//            float distance = std::sqrt((entityPos.x - target.x) * (entityPos.x - target.x) + (entityPos.y - target.y) * (entityPos.y - target.y));
//            if (distance < 5.0f) {
//                pathfinding.currentIndex++;
//            }
//        }
//    }
//}