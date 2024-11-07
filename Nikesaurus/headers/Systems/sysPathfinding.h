/*****************************************************************//**
 * \file   sysPathfinding.h
 * \brief  Pathfinding System
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (100%)
 * \date   November 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef PATHFINDING_HPP
#define PATHFINDING_HPP

#include "Managers/ECS/mSystem.h"
#include "Managers/Services/sMap.h"
#include "Components/cTransform.h"

//namespace NIKE {
//    namespace Pathfinding {
//
//        class Manager : public System::ISystem {
//        
//        private:
//            // Move entity towards player
//            void moveEntity(NIKE::Entity::Type entity);
//
//            // Map service for pathfinding logic (if still used)
//            //std::unique_ptr<MapService> mapService; 
//            
//        public:
//            Manager() = default;
//            ~Manager() = default;
//
//            // Initialize pathfinding system
//            void init() override;
//
//            // Update system (called every frame)
//            void update() override;
//
//            //System name
//            std::string getSysName() override
//            {
//                return "Pathfinding System";
//            }
//        };
//    } // namespace Pathfinding
//} // namespace NIKE

#endif //!PATHFINDING_HPP