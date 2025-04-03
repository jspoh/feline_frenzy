/*****************************************************************//**
 * \file   sysInteraction.h
 * \brief  Interaction system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (100%)
 * \date   November 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef INTERACTION_HPP
#define INTERACTION_HPP

#include "Managers/Services/sEvents.h"
#include "Managers/ECS/mEntity.h"
#include "Managers/ECS/mSystem.h"
#include "Components/cElement.h"

namespace NIKE {
    namespace Interaction {

        static std::unordered_map<Entity::Type, bool> hitEntities;

        static bool show_pause_menu;


        /*****************************************************************//**
         * UI Interactions
         *********************************************************************/
        void pauseOverlay(const std::string& background_texture, const std::string& resume, const std::string& settings, const std::string& how_to_play, const std::string& quit);

        // HARD CODED
        void gameOverlay(const std::string& background_texture, const std::string& play_again, const std::string& quit_game_text);


        /*****************************************************************//**
         * VFX and SFX Interactions
         *********************************************************************/

        void playSFX([[maybe_unused]] Entity::Type& entity, [[maybe_unused]] bool play_or_no);

        void animationSet(Entity::Type const& entity, int start_x, int start_y, int end_x, int end_y);

        void flipX(Entity::Type const& entity, bool yes_or_no);

        void flipY(Entity::Type const& entity, bool yes_or_no);

        void setLastDirection(Entity::Type const& entity, int dir);

        int getLastDirection(Entity::Type const& entity);

        // Spawn Heal Animation
        void spawnHealAnimation(Entity::Type player);

        /*****************************************************************//**
         * Player Interactions
         *********************************************************************/

        // Restore Health
        void restoreHealth(Entity::Type healer, Entity::Type target);

        // Handles interaction logic between two entities
        void handleCollision(Entity::Type entity_a, Entity::Type entity_b);

        // Apply damage
        void applyDamage(Entity::Type attacker, Entity::Type target);

        // Change element
        void changeElement(Entity::Type player, Entity::Type source);

        // For world-space calculation
        bool isWithinWorldRange(Entity::Type source, Entity::Type player);

        class Manager : 
            public System::ISystem {
        public:
            // Default constructor
            Manager() = default;

            // Default destructor
            ~Manager() = default;

            // Initialization
            void init() override;

            // Update
            void update() override;

            // System name
            std::string getSysName() override {
                return "Interaction System";
            }

        private:
            Manager(Manager const& copy) = delete;
            void operator=(Manager const& copy) = delete;

        };
    }
}


#endif //!INTERACTION_HPP