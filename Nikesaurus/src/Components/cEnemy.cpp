/*****************************************************************//**
 * \file   cEnemy.cpp
 * \brief  Enemy components
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cEnemy.h"

namespace NIKE {
	//int Enemy::Spawner::enemy_limit = 0;
	//int Enemy::Spawner::enemies_spawned = 0;

	void Enemy::registerComponents() {
		// Register attack components
		NIKE_ECS_MANAGER->registerComponent<Attack>();

		// Register spawner component
		NIKE_ECS_MANAGER->registerComponent <Spawner>();

		// Register attack for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Attack>(
			// Serialize
			[](Attack const& comp) -> nlohmann::json {
				return {
					{ "Range" , comp.range },
					{ "Cooldown", comp.cooldown },
					{ "LastShotTime", comp.last_shot_time },
					{ "Offset", comp.offset },
					{ "Layer", comp.layer },
					{ "PrefabPath", comp.prefab_path},
					{ "ScriptID", comp.script.script_id },
					{ "ScriptPath", comp.script.script_path },
					{ "Function", comp.script.function },
					{ "ScriptLoaded", comp.script.b_loaded },
				};
			},

			// Deserialize
			[](Attack& comp, nlohmann::json const& data) {
				comp.range = data.value("Range", 10.f);
				comp.cooldown = data.value("Cooldown", 10.f);
				comp.last_shot_time = data.value("LastShotTime", 10.f);
				comp.offset = data.value("Offset", 100.0f);
				comp.layer = data.value("Layer", 0);
				comp.prefab_path = data.value("PrefabPath", "");
				comp.script.script_id = data.value("ScriptID", "");
				comp.script.script_path = data.value("ScriptPath", "");
				comp.script.function = data.value("Function", "");
				comp.script.b_loaded = data.value("ScriptLoaded", false);
			},

			// Override Serialize
			[](Attack const& comp, Attack const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.range != other_comp.range) {
					delta["Range"] = comp.range;
				}
				if (comp.cooldown != other_comp.cooldown) {
					delta["Cooldown"] = comp.cooldown;
				}
				if (comp.last_shot_time != other_comp.last_shot_time) {
					delta["LastShotTime"] = comp.last_shot_time;
				}
				if (comp.offset != other_comp.offset) {
					delta["Offset"] = comp.offset;
				}
				if (comp.layer != other_comp.layer) {
					delta["Layer"] = comp.layer;
				}
				if (comp.prefab_path != other_comp.prefab_path) {
					delta["PrefabPath"] = comp.prefab_path;
				}
				if (comp.script.script_id != other_comp.script.script_id) {
					delta["ScriptID"] = comp.script.script_id;
				}
				if (comp.script.script_path != other_comp.script.script_path) {
					delta["ScriptPath"] = comp.script.script_path;
				}
				if (comp.script.function != other_comp.script.function) {
					delta["Function"] = comp.script.function;
				}
				if (comp.script.b_loaded != other_comp.script.b_loaded) {
					delta["ScriptLoaded"] = comp.script.b_loaded;
				}

				return delta;
			},

			// Override Deserialize
			[](Attack& comp, nlohmann::json const& delta) {
				if (delta.contains("Range")) {
					comp.range = delta["Range"];
				}
				if (delta.contains("Cooldown")) {
					comp.cooldown = delta["Cooldown"];
				}
				if (delta.contains("LastShotTime")) {
					comp.last_shot_time = delta["LastShotTime"];
				}
				if (delta.contains("Offset")) {
					comp.offset = delta["Offset"];
				}
				if (delta.contains("Layer")) {
					comp.layer = delta["Layer"];
				}
				if (delta.contains("PrefabPath")) {
					comp.prefab_path = delta["PrefabPath"];
				}

				// Nested script properties
				if (delta.contains("ScriptID")) {
					comp.script.script_id = delta["ScriptID"];
				}
				if (delta.contains("ScriptPath")) {
					comp.script.script_path = delta["ScriptPath"];
				}
				if (delta.contains("Function")) {
					comp.script.function = delta["Function"];
				}
				if (delta.contains("ScriptLoaded")) {
					comp.script.b_loaded = delta["ScriptLoaded"];
				}
			}
		);

		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Attack>();

		// Register spawner for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Spawner>(
			// Serialize
			[](Spawner const& comp) -> nlohmann::json {
				return {
					{ "EnemyElement", comp.enemy_element},
					{ "EnemyLimit", comp.enemy_limit},
					{ "EnemiesSpawned", comp.enemies_spawned},
					{ "Cooldown", comp.cooldown },
					{ "LastSpawnTime", comp.last_spawn_time }
				};
			},

			// Deserialize
			[](Spawner& comp, nlohmann::json const& data) {
				comp.enemy_element = data.at("EnemyElement").get<Element::Elements>();
				comp.enemy_limit = data.at("EnemyLimit").get<int>();
				comp.enemies_spawned = data.at("EnemiesSpawned").get<int>();
				comp.cooldown = data.at("Cooldown").get<float>();
				comp.last_spawn_time = data.at("LastSpawnTime").get<float>();
			},

			// Override Serialize for Spawner
			[](Spawner const& comp, Spawner const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.enemy_element != other_comp.enemy_element) {
					delta["EnemyElement"] = comp.enemy_element;
				}
				if (comp.enemy_limit != other_comp.enemy_limit) {
					delta["EnemyLimit"] = comp.enemy_limit;
				}
				if (comp.enemies_spawned != other_comp.enemies_spawned) {
					delta["EnemiesSpawned"] = comp.enemies_spawned;
				}
				if (comp.cooldown != other_comp.cooldown) {
					delta["Cooldown"] = comp.cooldown;
				}
				if (comp.last_spawn_time != other_comp.last_spawn_time) {
					delta["LastSpawnTime"] = comp.last_spawn_time;
				}

				return delta;
			},

			// Override Deserialize for Spawner
			[](Spawner& comp, nlohmann::json const& delta) {
				if (delta.contains("EnemyElement")) {
					comp.enemy_element = delta["EnemyElement"];
				}
				if (delta.contains("EnemyLimit")) {
					comp.enemy_limit = delta["EnemyLimit"];
				}
				if (delta.contains("EnemiesSpawned")) {
					comp.enemies_spawned = delta["EnemiesSpawned"];
				}
				if (delta.contains("Cooldown")) {
					comp.cooldown = delta["Cooldown"];
				}
				if (delta.contains("LastSpawnTime")) {
					comp.last_spawn_time = delta["LastSpawnTime"];
				}
			}
		);

		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Spawner>();
	}

	void Enemy::registerEditorComponents() {
#ifndef NDEBUG
		// Level Editor UI registration
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Attack>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Attack& comp) {

				ImGui::Text("Edit Shooting Variables:");

				// For attack range
				{
					static float before_change_range;

					ImGui::DragFloat("Range", &comp.range, 0.1f);

					// Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_range = comp.range;
					}

					// Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_range;

						// Change do action
						change_range.do_action = [&, range = comp.range]() {
							comp.range = range;
							};

						// Change undo action
						change_range.undo_action = [&, range = before_change_range]() {
							comp.range = range;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_range));
					}
				}

				// For shooting cooldown
				{
					static float before_change_cooldown;

					ImGui::DragFloat("Shot Cooldown", &comp.cooldown, 0.1f);

					// Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_cooldown = comp.cooldown;
					}

					// Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_cooldown;

						// Change do action
						change_cooldown.do_action = [&, cooldown = comp.cooldown]() {
							comp.cooldown = cooldown;
							};

						// Change undo action
						change_cooldown.undo_action = [&, cooldown = before_change_cooldown]() {
							comp.cooldown = cooldown;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_cooldown));
					}
				}

				// Show and allow editing of the offset
				{
					static float before_change_offset;

					ImGui::DragFloat("Shot Offset", &comp.offset, 0.1f);

					// Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_offset = comp.offset;
					}

					// Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_offset;

						// Change do action
						change_offset.do_action = [&, offset = comp.offset]() {
							comp.offset = offset;
							};

						// Change undo action
						change_offset.undo_action = [&, offset = before_change_offset]() {
							comp.offset = offset;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_offset));
					}
				}

				// Show and allow editing of the layer
				{
					static int before_change_layer;

					ImGui::DragInt("Layer", &comp.layer, 1);

					// Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_layer = comp.layer;
					}

					// Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_layer;

						// Change do action
						change_layer.do_action = [&, layer = comp.layer]() {
							comp.layer = layer;
							};

						// Change undo action
						change_layer.undo_action = [&, layer = before_change_layer]() {
							comp.layer = layer;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_layer));
					}
				}

				// !TODO: Change this to input
				ImGui::Text("Prefab Path: %s", comp.prefab_path.c_str());
				ImGui::Text("Script Path: %s", comp.script.script_path.c_str());
				ImGui::Text("Function: %s", comp.script.function.c_str());
				ImGui::Text("Script Loaded: %s", comp.script.b_loaded ? "Yes" : "No");
				ImGui::Text("Last Shot time: %f", comp.last_shot_time);
			}
		);
#endif

#ifndef NDEBUG
// Level Editor UI registration
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Spawner>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Spawner& comp) {

				ImGui::Text("Edit Spawner Variables:");

				// For Enemy Element
				{
					ImGui::Text("Enemy Element:");
					static const char* elements_names[] = { "RANDOM", "FIRE", "WATER", "GRASS" };

					static Element::Elements before_change_element;
					static int previous_element = static_cast<int>(comp.enemy_element);
					int current_element = static_cast<int>(comp.enemy_element);

					if (ImGui::Combo("##Element", &current_element, elements_names, IM_ARRAYSIZE(elements_names))) {
						Element::Elements new_element = static_cast<Element::Elements>(current_element);
						if (new_element != comp.enemy_element) {
							// Save action
							LevelEditor::Action save_element;
							save_element.do_action = [&, enemy_element = new_element]() {
								comp.enemy_element = enemy_element;
								};

							// Undo action
							save_element.undo_action = [&, enemy_element = before_change_element]() {
								comp.enemy_element = enemy_element;
								};

							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_element));

							// Update the previous value
							before_change_element = comp.enemy_element;
							// Apply the new element
							comp.enemy_element = new_element;
						}
					}
				}
				// For Enemy Limit
				{
					static int before_change_limit;

					ImGui::DragInt("Enemy Limit", &comp.enemy_limit, 1);

					// Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_limit = comp.enemy_limit;
					}

					// Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_limit;

						// Change do action
						change_limit.do_action = [&, enemy_limit = comp.enemy_limit]() {
							comp.enemy_limit = enemy_limit;
							};

						// Change undo action
						change_limit.undo_action = [&, enemy_limit = before_change_limit]() {
							comp.enemy_limit = enemy_limit;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_limit));
					}
				}

				// For Enemies Spawned
				{
					static int before_change_spawned;

					ImGui::DragInt("Enemies Spawned", &comp.enemies_spawned, 1);

					// Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_spawned = comp.enemies_spawned;
					}

					// Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_spawned;

						// Change do action
						change_spawned.do_action = [&, enemies_spawned = comp.enemies_spawned]() {
							comp.enemies_spawned = enemies_spawned;
							};

						// Change undo action
						change_spawned.undo_action = [&, enemies_spawned = before_change_spawned]() {
							comp.enemies_spawned = enemies_spawned;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_spawned));
					}
				}

				// Spawning Cooldopwn
				{
					static float before_change_spawn_cooldown;

					ImGui::DragFloat("Spawn Cooldown", &comp.cooldown, 0.1f);

					// Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_spawn_cooldown = comp.cooldown;
					}

					// Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_spawn_cooldown;

						// Change do action
						change_spawn_cooldown.do_action = [&, cooldown = comp.cooldown]() {
							comp.cooldown = cooldown;
							};

						// Change undo action
						change_spawn_cooldown.undo_action = [&, cooldown = before_change_spawn_cooldown]() {
							comp.cooldown = cooldown;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_spawn_cooldown));
					}
				}

				// Last Spawn Time
				ImGui::Text("Last Spawn Time: %f", comp.last_spawn_time);
			}
		);
#endif
	}
}