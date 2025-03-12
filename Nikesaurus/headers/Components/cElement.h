/*****************************************************************//**
 * \file   cElement.h
 * \brief  Defines the components used for Elements
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *********************************************************************/

#pragma once

#ifndef C_ELEMENT_HPP
#define C_ELEMENT_HPP

#include "Core/stdafx.h"

namespace NIKE {
	namespace Element {
		const enum class Elements : int {
			NONE = 0,
			FIRE,
			WATER,
			GRASS,
		};

		const enum class Status : int {
			NONE = 0,
			BURN,
			FREEZE,
			POISON,
		};

		constexpr float elemental_multiplier_table[4][4] = {
			//			NONE  FIRE   WATER  GRASS
			/* NONE */ { 1.0f, 1.0f, 1.0f, 1.0f },
			/* FIRE */ { 1.0f, 1.0f, 0.5f, 2.0f },
			/* WATER */ { 1.0f, 2.0f, 1.0f, 0.5f },
			/* GRASS */ { 1.0f, 0.5f, 2.0f, 1.0f }
		};


		struct Entity {
			Elements element;			// Currently equipped element
			// !TODO: Maybe make it random at the start?

			Entity() : element(Elements::NONE) {};
			Entity(int element)
				: element{ element } {}

			static const std::string enemyBullet[4];	// Array of enemy elemental bullet
			static const std::string playerBullet[4];	// Array of elemental bullets (For shooting)
		};

		struct Source {
			Elements element;			// Currently equipped element

			Source() : element(Elements::NONE) {};
			Source(int element)
				: element{ element } {}
		};

		struct Combo {
			std::deque<Elements> last_hits;			// Last 3 elements that hit the entity
			static constexpr size_t max_size = 3;	// Max number of hits we are tracking

			Status status_effect;					// Whether affected by status effect
			float status_timer;						// Current status effect duration
			float tick_timer;						// Timer for status effect tick
			float temp_max_speed;					// Temporary max speed for slow effect

			static constexpr int ticks_per_status = 4;		 // Number of ticks per status application
			static constexpr float status_duration = 2.f;	 // Total status effect time
			static constexpr float tick_interval = status_duration/ticks_per_status;     // Time between ticks

			Combo() : last_hits{}, status_effect(Status::NONE), status_timer(0.f), tick_timer(0.f), temp_max_speed(-1.f)  {};

			void registerHit(Elements element) {
				// If queue is full (queue full of random elements)
				if (last_hits.size() == max_size) {
					NIKEE_CORE_INFO("queue full, {} popped", static_cast<int>(last_hits.front()));
					// Remove the oldest hit
					last_hits.pop_front();  
				}

				// Add new element to queue
				NIKEE_CORE_INFO("{} added to queue", static_cast<int>(element));
				last_hits.push_back(element); 

				// Apply Status Effect
				applyStatusEffect();
			}

			void applyStatusEffect() {
				// Ensure 3 hits exist
				if (last_hits.size() < max_size) {
					return;
				}

				// If all elements in the queue are the same
				if (last_hits[0] == last_hits[1] && last_hits[1] == last_hits[2]) {
					switch (last_hits[0]) {
					case Elements::FIRE:
						status_effect = Status::BURN;
						break;
					case Elements::WATER:
						status_effect = Status::FREEZE;
						break;
					case Elements::GRASS:
						status_effect = Status::POISON;
						break;
					default:
						status_effect = Status::NONE;
						break;
					}

					NIKEE_CORE_INFO("{} status applied", static_cast<int>(status_effect));

					// Clear queue after inflicting status
					while (!last_hits.empty()) {
						last_hits.pop_back();
						NIKEE_CORE_INFO("Queue popped");
					}

					// Reset status effect duration
					status_timer = status_duration;  
				}
			}

			void applyBurn(float& health, float burn_damage) {
				// Decrement by burn ammount
				health = max(0.0f, health - burn_damage);
			}

			void applyFreeze(float& max_speed, float freeze_speed) {
				// Only store max speed once
				if (temp_max_speed < 0) { 
					temp_max_speed = max_speed;
				}
				// Setting max speed to lowered speed
				max_speed = freeze_speed;
			}
		};

		const std::string playerBullet[4] = { "bullet.prefab", "fireBullet.prefab", "waterBullet.prefab", "grassBullet.prefab" };
		const std::string enemyBullet[4] = { "enemyBullet.prefab", "enemyFireBullet.prefab", "enemyWaterBullet.prefab", "enemyGrassBullet.prefab" };

		//const std::string elementUI[4] = { "Water icon grey.png", "Fire icon.png", "Water icon.png", "Grass icon.png" };

		// Get Multiplier for Elemental Damage
		float getElementMultiplier(Element::Elements attacker, Element::Elements defender);

		void registerComponents();

		void registerEditorComponents();
	}
}


#endif // !C_ELEMENT_HPP