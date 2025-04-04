/*****************************************************************//**
 * \file   cRender.cpp
 * \brief	Render components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (50%)
 * \co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (50%)
 * \date   October 2024
 *  * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cRender.h"
#include "Managers/Services/Render/sParticle.h"

namespace NIKE {

	//Construct particle emitter
	Render::ParticleEmitter::ParticleEmitter() : offset{ 0.f, 0.f }, render_type{ 0 }, preset{ 0 }, ref{ "" }, duration{ -1.f } {
		p_system = std::make_shared<SysParticle::ParticleSystem>();
	}

	//Definition for video component static channel group
	std::shared_ptr<Audio::IChannelGroup> Render::Video::channel_group = nullptr;

	void Render::registerComponents() {

		//Register render components
		NIKE_ECS_MANAGER->registerComponent<Render::Cam>();
		NIKE_ECS_MANAGER->registerComponent<Render::Text>();
		NIKE_ECS_MANAGER->registerComponent<Render::Shape>();
		NIKE_ECS_MANAGER->registerComponent<Render::Texture>();
		NIKE_ECS_MANAGER->registerComponent<Render::ParticleEmitter>();
		NIKE_ECS_MANAGER->registerComponent<Render::Video>();

		NIKE_SERIALIZE_SERVICE->registerComponent<Render::ParticleEmitter>(
			//Serialize
			[](Render::ParticleEmitter const& comp) -> nlohmann::json {
				return	{
					{ "preset", static_cast<int>(comp.p_system->preset) },
					{ "render_type", static_cast<int>(comp.p_system->render_type) },
					{ "offset", comp.offset.toJson() },
					{ "duration", comp.p_system->duration },

					{ "num_new_particles_per_second", comp.p_system->num_new_particles_per_second },
					{ "particle_lifespan", comp.p_system->particle_lifespan },
					{ "particle_acceleration", comp.p_system->particle_acceleration },
					{ "particle_velocity_range", comp.p_system->particle_velocity_range.toJson() },
					{ "particle_vector_x_range", comp.p_system->particle_vector_x_range.toJson() },
					{ "particle_vector_y_range", comp.p_system->particle_vector_y_range.toJson() },
					{ "particle_color_is_random", comp.p_system->particle_color_is_random },
					{ "particle_color", comp.p_system->particle_color.toJson()},
					{ "particle_rand_x_offset_range", comp.p_system->particle_rand_x_offset_range.toJson()},
					{ "particle_rand_y_offset_range", comp.p_system->particle_rand_y_offset_range.toJson()},
					{ "particle_rotation", comp.p_system->particle_rotation },
					{ "particle_rand_width_range", comp.p_system->particle_rand_width_range.toJson()},

					{ "particle_size_changes_over_time", comp.p_system->particle_size_changes_over_time },
					{ "particle_final_size", comp.p_system->particle_final_size.toJson()},
					{ "particle_color_changes_over_time", comp.p_system->particle_color_changes_over_time },
					{ "particle_final_color", comp.p_system->particle_final_color.toJson()},
					{ "particle_rotation_speed", comp.p_system->particle_rotation_speed },

					{ "texture_ref", comp.p_system->texture_ref }

					//{ "ref", comp.ref }
				};
			},
			//Deserialize
			[](Render::ParticleEmitter& comp, nlohmann::json const& data) {

				//Initialize particle system
				try {
					comp.offset.fromJson(data.at("offset"));
					comp.duration = data.at("duration").get<float>();
					comp.preset = comp.preset;
					comp.duration = data.at("duration").get<float>();
					comp.render_type = data.at("render_type").get<int>();

					comp.num_new_particles_per_second = data.at("num_new_particles_per_second").get<int>();
					comp.particle_lifespan = data.at("particle_lifespan").get<float>();
					comp.particle_acceleration = data.at("particle_acceleration").get<float>();
					comp.particle_velocity_range.fromJson(data.at("particle_velocity_range"));
					comp.particle_vector_x_range.fromJson(data.at("particle_vector_x_range"));
					comp.particle_vector_y_range.fromJson(data.at("particle_vector_y_range"));
					comp.particle_color_is_random = data.at("particle_color_is_random").get<bool>();
					comp.particle_color.fromJson(data.at("particle_color"));
					comp.particle_rand_x_offset_range.fromJson(data.at("particle_rand_x_offset_range"));
					comp.particle_rand_y_offset_range.fromJson(data.at("particle_rand_y_offset_range"));
					comp.particle_rotation = data.at("particle_rotation").get<float>();
					comp.particle_rand_width_range.fromJson(data.at("particle_rand_width_range"));
					comp.particle_size_changes_over_time = data.at("particle_size_changes_over_time").get<bool>();
					comp.particle_final_size.fromJson(data.at("particle_final_size"));
					comp.particle_color_changes_over_time = data.at("particle_color_changes_over_time").get<bool>();
					comp.particle_final_color.fromJson(data.at("particle_final_color"));
					comp.particle_rotation_speed = data.at("particle_rotation_speed").get<float>();
					comp.texture_ref = data.at("texture_ref").get<std::string>();
					
					comp.p_system->particles.reserve(SysParticle::MAX_PARTICLE_SYSTEM_ACTIVE_PARTICLES);

					comp.p_system->preset = static_cast<SysParticle::Data::ParticlePresets>(comp.preset);
					comp.p_system->origin = comp.offset; // Will be updated to proper origin through the particle update function
					comp.p_system->is_alive = true;
					comp.p_system->duration = comp.duration;
					comp.p_system->time_alive = 0.f;
					comp.p_system->using_world_pos = true; // Temporary set to true
					comp.p_system->render_type = static_cast<SysParticle::Data::ParticleRenderType>(comp.render_type);

					//	comp.p_system->num_new_particles_per_second
					//	comp.p_system->particle_lifespan
					//	comp.p_system->particle_acceleration
					//	comp.p_system->particle_velocity_range
					//	comp.p_system->particle_vector_x_range
					//	comp.p_system->particle_vector_y_range
					//	comp.p_system->particle_color_is_random
					//	comp.p_system->particle_color
					//	comp.p_system->particle_rand_x_offset_range
					//	comp.p_system->particle_rand_y_offset_range
					//	comp.p_system->particle_rotation
					//	comp.p_system->particle_rand_width_range
					//	comp.p_system->particle_size_changes_over_time
					//	comp.p_system->particle_final_size
					//	comp.p_system->particle_color_changes_over_time
					//	comp.p_system->particle_color_changes_over_time
					//	comp.p_system->particle_final_color
					//	comp.p_system->particle_rotation_speed

				}
				catch (std::exception& e) {
					(void)e;
				}
			},
			// Override Serialize
			[](Render::ParticleEmitter const& comp, Render::ParticleEmitter const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.p_system->preset != other_comp.p_system->preset) {
					delta["preset"] = comp.p_system->preset;
				}
				if (comp.p_system->render_type != other_comp.p_system->render_type) {
					delta["render_type"] = comp.p_system->render_type;
				}
				if (comp.offset != other_comp.offset) {
					delta["offset"] = comp.offset.toJson();
				}
				if (comp.p_system->duration != other_comp.p_system->duration) {
					delta["duration"] = comp.p_system->duration;
				}
				//if (comp.p_system->ref != other_comp.p_system->ref) {
				//	delta["ref"] = comp.p_system->ref;
				//}

				if (comp.p_system->num_new_particles_per_second != other_comp.p_system->num_new_particles_per_second) {
					delta["num_new_particles_per_second"] = comp.p_system->num_new_particles_per_second;
				}
				if (comp.p_system->particle_lifespan != other_comp.p_system->particle_lifespan) {
					delta["particle_lifespan"] = comp.p_system->particle_lifespan;
				}
				if (comp.p_system->particle_acceleration != other_comp.p_system->particle_acceleration) {
					delta["particle_acceleration"] = comp.p_system->particle_acceleration;
				}
				if (comp.p_system->particle_velocity_range != other_comp.p_system->particle_velocity_range) {
					delta["particle_velocity_range"] = comp.p_system->particle_velocity_range.toJson();
				}
				if (comp.p_system->particle_vector_x_range != other_comp.p_system->particle_vector_x_range) {
					delta["particle_vector_x_range"] = comp.p_system->particle_vector_x_range.toJson();
				}
				if (comp.p_system->particle_vector_y_range != other_comp.p_system->particle_vector_y_range) {
					delta["particle_vector_y_range"] = comp.p_system->particle_vector_y_range.toJson();
				}
				if (comp.p_system->particle_color_is_random != other_comp.p_system->particle_color_is_random) {
					delta["particle_color_is_random"] = comp.p_system->particle_color_is_random;
				}
				if (comp.p_system->particle_color != other_comp.p_system->particle_color) {
					delta["particle_color"] = comp.p_system->particle_color.toJson();
				}
				if (comp.p_system->particle_rand_x_offset_range != other_comp.p_system->particle_rand_x_offset_range) {
					delta["particle_rand_x_offset_range"] = comp.p_system->particle_rand_x_offset_range.toJson();
				}
				if (comp.p_system->particle_rand_y_offset_range != other_comp.p_system->particle_rand_y_offset_range) {
					delta["particle_rand_y_offset_range"] = comp.p_system->particle_rand_y_offset_range.toJson();
				}
				if (comp.p_system->particle_rotation != other_comp.p_system->particle_rotation) {
					delta["particle_rotation"] = comp.p_system->particle_rotation;
				}
				if (comp.p_system->particle_rand_width_range != other_comp.p_system->particle_rand_width_range) {
					delta["particle_rand_width_range"] = comp.p_system->particle_rand_width_range.toJson();
				}

				if (comp.p_system->particle_size_changes_over_time != other_comp.p_system->particle_size_changes_over_time) {
					delta["particle_size_changes_over_time"] = comp.p_system->particle_size_changes_over_time;
				}
				if (comp.p_system->particle_final_size != other_comp.p_system->particle_final_size) {
					delta["particle_final_size"] = comp.p_system->particle_final_size.toJson();
				}
				if (comp.p_system->particle_color_changes_over_time != other_comp.p_system->particle_color_changes_over_time) {
					delta["particle_color_changes_over_time"] = comp.p_system->particle_color_changes_over_time;
				}
				if (comp.p_system->particle_final_color != other_comp.p_system->particle_final_color) {
					delta["particle_final_color"] = comp.p_system->particle_final_color.toJson();
				}
				if (comp.p_system->particle_rotation_speed != other_comp.p_system->particle_rotation_speed) {
					delta["particle_rotation_speed"] = comp.p_system->particle_rotation_speed;
				}
				if (comp.p_system->texture_ref != other_comp.p_system->texture_ref) {
					delta["texture_ref"] = comp.p_system->texture_ref;
				}

				return delta;
			},
			// Override Deserialize
			[](Render::ParticleEmitter& comp, nlohmann::json const& delta) {
				if (delta.contains("preset")) {
					comp.p_system->preset = delta["preset"];
				}
				if (delta.contains("render_type")) {
					comp.p_system->render_type = delta["render_type"];
				}
				if (delta.contains("offset")) {
					comp.offset.fromJson(delta["offset"]);
				}
				if (delta.contains("duration")) {
					comp.p_system->duration = delta["duration"];
				}

				if (delta.contains("num_new_particles_per_second")) {
					comp.p_system->num_new_particles_per_second = delta["num_new_particles_per_second"];
				}
				if (delta.contains("particle_lifespan")) {
					comp.p_system->particle_lifespan = delta["particle_lifespan"];
				}
				if (delta.contains("particle_acceleration")) {
					comp.p_system->particle_acceleration = delta["particle_acceleration"];
				}
				if (delta.contains("particle_velocity_range")) {
					comp.p_system->particle_velocity_range.fromJson(delta["particle_velocity_range"]);
				}
				if (delta.contains("particle_vector_x_range")) {
					comp.p_system->particle_vector_x_range.fromJson(delta["particle_vector_x_range"]);
				}
				if (delta.contains("particle_vector_y_range")) {
					comp.p_system->particle_vector_y_range.fromJson(delta["particle_vector_y_range"]);
				}
				if (delta.contains("particle_color_is_random")) {
					comp.p_system->particle_color_is_random = delta["particle_color_is_random"];
				}
				if (delta.contains("particle_color")) {
					comp.p_system->particle_color.fromJson(delta["particle_color"]);
				}
				if (delta.contains("particle_rand_x_offset_range")) {
					comp.p_system->particle_rand_x_offset_range.fromJson(delta["particle_rand_x_offset_range"]);
				}
				if (delta.contains("particle_rand_y_offset_range")) {
					comp.p_system->particle_rand_y_offset_range.fromJson(delta["particle_rand_y_offset_range"]);
				}
				if (delta.contains("particle_rotation")) {
					comp.p_system->particle_rotation = delta["particle_rotation"];
				}
				if (delta.contains("particle_rand_width_range")) {
					comp.p_system->particle_rand_width_range.fromJson(delta["particle_rand_width_range"]);
				}

				if (delta.contains("particle_size_changes_over_time")) {
					comp.p_system->particle_size_changes_over_time = delta["particle_size_changes_over_time"];
				}
				if (delta.contains("particle_final_size")) {
					comp.p_system->particle_final_size.fromJson(delta["particle_final_size"]);
				}
				if (delta.contains("particle_color_changes_over_time")) {
					comp.p_system->particle_color_changes_over_time = delta["particle_color_changes_over_time"];
				}
				if (delta.contains("particle_final_color")) {
					comp.p_system->particle_final_color.fromJson(delta["particle_final_color"]);
				}
				if (delta.contains("particle_rotation_speed")) {
					comp.p_system->particle_rotation_speed = delta["particle_rotation_speed"];
				}
				if (delta.contains("texture_ref")) {
					comp.p_system->texture_ref = delta["texture_ref"];
				}

				// add particle system
				//NIKE::SysParticle::Manager::getInstance().addActiveParticleSystem(particle_emitter_ref, NIKE::SysParticle::Data::ParticlePresets(comp.p_system->preset), comp.p_system->offset, static_cast<NIKE::SysParticle::Data::ParticleRenderType>(comp.p_system->render_type), comp.p_system->duration);
			}
		);

		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Render::ParticleEmitter>();

		//Register cam for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Render::Cam>(
			//Serialize
			[](Render::Cam const& comp) -> nlohmann::json {
				return	{
						{ "Position", comp.position.toJson() },
						{ "Zoom", comp.zoom },
						{ "Mouse_Offset", comp.mouse_offset}
				};
			},

			//Deserialize
			[](Render::Cam& comp, nlohmann::json const& data) {
				comp.position.fromJson(data.value("Position", Vector2f::def_json));
				comp.zoom = data.value("Zoom", 1.0f);
				comp.mouse_offset = data.value("Mouse_Offset", 0.f);
			},

			// Override Serialize
			[](Render::Cam const& comp, Render::Cam const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.position != other_comp.position) {
					delta["Position"] = comp.position.toJson();
				}
				if (comp.zoom != other_comp.zoom) {
					delta["Zoom"] = comp.zoom;
				}
				if (comp.mouse_offset != other_comp.mouse_offset) {
					delta["Mouse_Offset"] = comp.mouse_offset;
				}

				return delta;
			},

			// Override Deserialize
			[](Render::Cam& comp, nlohmann::json const& delta) {
				if (delta.contains("Position")) {
					comp.position.fromJson(delta["Position"]);
				}
				if (delta.contains("Zoom")) {
					comp.zoom = delta["Zoom"];
				}
				if (delta.contains("Mouse_Offset")) {
					comp.mouse_offset = delta["Mouse_Offset"];
				}
			}
		);

		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Render::Cam>();

		//Register text for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Render::Text>(
			//Serialize
			[](Render::Text const& comp) -> nlohmann::json {
				return	{
						{ "Font_ID", comp.font_id },
						{ "Text", comp.text },
						{ "Color", comp.color.toJson() },
						{ "Scale", comp.scale },
						{ "Size", comp.size.toJson() },
						{ "Origin", static_cast<int>(comp.origin) }
				};
			},

			//Deserialize
			[](Render::Text& comp, nlohmann::json const& data) {
				comp.font_id = data.value("Font_ID", "");
				comp.text = data.value("Text", "");
				comp.color.fromJson(data.value("Color", Vector4f::def_json));
				comp.scale = data.value("Scale", 1.0f);
				comp.size.fromJson(data.value("Size", Vector2f::def_json));
				comp.origin = data.value("Origin", TextOrigin::CENTER);
			},

			// Override Serialize
			[](Render::Text const& comp, Render::Text const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.font_id != other_comp.font_id) {
					delta["Font_ID"] = comp.font_id;
				}
				if (comp.text != other_comp.text) {
					delta["Text"] = comp.text;
				}
				if (comp.color != other_comp.color) {
					delta["Color"] = comp.color.toJson();
				}
				if (comp.scale != other_comp.scale) {
					delta["Scale"] = comp.scale;
				}
				if (comp.size != other_comp.size) {
					delta["Size"] = comp.size.toJson();
				}
				if (comp.origin != other_comp.origin) {
					delta["Origin"] = static_cast<int>(comp.origin);
				}

				return delta;
			},

			// Override Deserialize
			[](Render::Text& comp, nlohmann::json const& delta) {
				if (delta.contains("Font_ID")) {
					comp.font_id = delta["Font_ID"];
				}
				if (delta.contains("Text")) {
					comp.text = delta["Text"];
				}
				if (delta.contains("Color")) {
					comp.color.fromJson(delta["Color"]);
				}
				if (delta.contains("Scale")) {
					comp.scale = delta["Scale"];
				}
				if (delta.contains("Size")) {
					comp.size.fromJson(delta["Size"]);
				}
				if (delta.contains("Origin")) {
					comp.origin = static_cast<TextOrigin>(delta["Origin"]);
				}
			}
		);

		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Render::Text>();

		//Register shape for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Render::Shape>(
			//Serialize
			[](Render::Shape const& comp) -> nlohmann::json {
				return	{
						{ "Model_ID", comp.model_id },
						{ "Color", comp.color.toJson() }
				};
			},

			//Deserialize
			[](Render::Shape& comp, nlohmann::json const& data) {
				comp.model_id = data.value("Model_ID", "");
				comp.color.fromJson(data.value("Color", Vector4f::def_json));
			},

			// Override Serialize
			[](Render::Shape const& comp, Render::Shape const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.model_id != other_comp.model_id) {
					delta["Model_ID"] = comp.model_id;
				}
				if (comp.color != other_comp.color) {
					delta["Color"] = comp.color.toJson();
				}

				return delta;
			},

			// Override Deserialize
			[](Render::Shape& comp, nlohmann::json const& delta) {
				if (delta.contains("Model_ID")) {
					comp.model_id = delta["Model_ID"];
				}
				if (delta.contains("Color")) {
					comp.color.fromJson(delta["Color"]);
				}
			}
		);

		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Render::Shape>();

		//Register texture for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Render::Texture>(
			//Serialize
			[](Render::Texture const& comp) -> nlohmann::json {
				return	{
						{ "Texture_ID", comp.texture_id },
						{ "Color", comp.color.toJson() },
						{ "Frame_Size", comp.frame_size.toJson() },
						{ "Frame_Index", comp.frame_index.toJson() },
						{ "B_Blend", comp.b_blend },
						{ "Intensity", comp.intensity },
						{ "B_Stretch", comp.b_stretch },
						{ "B_Flip", comp.b_flip.toJson() }
				};
			},

			//Deserialize
			[](Render::Texture& comp, nlohmann::json const& data) {
				comp.texture_id = data.value("Texture_ID", "");
				comp.color.fromJson(data.value("Color", Vector4f::def_json));
				comp.frame_size.fromJson(data.value("Frame_Size", Vector2i::def_json));
				comp.frame_index.fromJson(data.value("Frame_Index", Vector2i::def_json));
				comp.b_blend = data.value("B_Blend", false);
				comp.intensity = data.value("Intensity", 0.5f);
				comp.b_stretch = data.value("B_Stretch", false);
				comp.b_flip.fromJson(data.value("B_Flip", Vector2b::def_json));
			},

			// Override Serialize
			[](Render::Texture const& comp, Render::Texture const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.texture_id != other_comp.texture_id) {
					delta["Texture_ID"] = comp.texture_id;
				}
				if (comp.color != other_comp.color) {
					delta["Color"] = comp.color.toJson();
				}
				if (comp.frame_size != other_comp.frame_size) {
					delta["Frame_Size"] = comp.frame_size.toJson();
				}
				if (comp.frame_index != other_comp.frame_index) {
					delta["Frame_Index"] = comp.frame_index.toJson();
				}
				if (comp.b_blend != other_comp.b_blend) {
					delta["B_Blend"] = comp.b_blend;
				}
				if (comp.intensity != other_comp.intensity) {
					delta["Intensity"] = comp.intensity;
				}
				if (comp.b_stretch != other_comp.b_stretch) {
					delta["B_Stretch"] = comp.b_stretch;
				}
				if (comp.b_flip != other_comp.b_flip) {
					delta["B_Flip"] = comp.b_flip.toJson();
				}

				return delta;
			},

			// Override Deserialize
			[](Render::Texture& comp, nlohmann::json const& delta) {
				if (delta.contains("Texture_ID")) {
					comp.texture_id = delta["Texture_ID"];
				}
				if (delta.contains("Color")) {
					comp.color.fromJson(delta["Color"]);
				}
				if (delta.contains("Frame_Size")) {
					comp.frame_size.fromJson(delta["Frame_Size"]);
				}
				if (delta.contains("Frame_Index")) {
					comp.frame_index.fromJson(delta["Frame_Index"]);
				}
				if (delta.contains("B_Blend")) {
					comp.b_blend = delta["B_Blend"];
				}
				if (delta.contains("Intensity")) {
					comp.intensity = delta["Intensity"];
				}
				if (delta.contains("B_Stretch")) {
					comp.b_stretch = delta["B_Stretch"];
				}
				if (delta.contains("B_Flip")) {
					comp.b_flip.fromJson(delta["B_Flip"]);
				}
			}
		);

		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Render::Texture>();

		//Register texture for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Render::Video>(
			//Serialize
			[](Render::Video const& comp) -> nlohmann::json {
				return	{
						{ "Video_ID", comp.video_id },
						{ "B_Loop", comp.b_loop }
				};
			},

			//Deserialize
			[](Render::Video& comp, nlohmann::json const& data) {
				comp.video_id = data.value("Video_ID", "");
				comp.b_loop = data.value("B_Loop", true);
			},

			// Override Serialize
			[](Render::Video const& comp, Render::Video const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.video_id != other_comp.video_id) {
					delta["Video_ID"] = comp.video_id;
				}

				if (comp.b_loop != other_comp.b_loop) {
					delta["B_Loop"] = comp.b_loop;
				}

				return delta;
			},

			// Override Deserialize
			[](Render::Video& comp, nlohmann::json const& delta) {
				if (delta.contains("B_Loop")) {
					comp.b_loop = delta["B_Loop"];
				}
			}
		);

		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Render::Video>();
	}

	void Render::registerEditorComponents() {

#ifndef NDEBUG
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Render::ParticleEmitter>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Render::ParticleEmitter& comp) {
				{
					// Before change
					static int before_change_preset;
					static Vector2f before_change_offset;
					static int before_change_num_new_particles_per_second;
					static float before_change_particle_lifespan;
					static float before_change_particle_acceleration;
					static Vector2f before_change_particle_velocity_range;
					static Vector2f before_change_particle_vector_x_range;
					static Vector2f before_change_particle_vector_y_range;
					static bool before_change_particle_color_is_random;
					static Vector4f before_change_particle_color;
					static Vector2f before_change_particle_rand_x_offset_range;
					static Vector2f before_change_particle_rand_y_offset_range;
					static float before_change_particle_rotation;
					static Vector2f before_change_particle_rand_width_range;
					static bool before_change_particle_final_size_changes_over_time;
					static float before_change_particle_final_size;
					static bool before_particle_color_changes_over_time;
					static Vector4f before_change_particle_final_color;
					static float before_change_particle_rotation_speed;

					// Preset
					ImGui::Text("Particle Preset:");

					std::string preset_options{};
					for (const auto& [preset, preset_ref] : SysParticle::Data::particle_preset_map) {
						preset_options += preset_ref + '\0';
					}
					preset_options += '\0';

					int selected_preset = static_cast<int>(comp.preset);
					static int previous_preset = selected_preset;
					if (ImGui::Combo("##Preset", &selected_preset, preset_options.c_str())) {
						// If the value changed, process the change
						if (selected_preset != previous_preset) {
							previous_preset = selected_preset;

							LevelEditor::Action change_preset;

							// Store the value before the change
							change_preset.do_action = [&, preset = selected_preset]() {
								comp.preset = preset;
								
								//Clear all particles
								comp.p_system->particles.clear();
								};

							// Store the undo action
							change_preset.undo_action = [&, preset = previous_preset]() {
								comp.preset = preset;

								//Clear all particles
								comp.p_system->particles.clear();
								};

							// Execute the action
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_preset));
						}
					}

					// render type
					{
						ImGui::Text("Particle Render Type:");
						std::string render_type_options{};
						for (const auto& [render_type, render_type_ref] : SysParticle::Data::particle_render_type_map) {
							render_type_options += render_type_ref + '\0';
						}
						render_type_options += '\0';

						int selected_render_type = static_cast<int>(comp.render_type);
						static int previous_render_type = selected_render_type;
						if (ImGui::Combo("##Render Type", &selected_render_type, render_type_options.c_str())) {
							// If the value changed, process the change
							if (selected_render_type != previous_render_type) {
								previous_render_type = selected_render_type;
								LevelEditor::Action change_render_type;
								// Store the value before the change
								change_render_type.do_action = [&, render_type = selected_render_type]() {
									comp.render_type = render_type;

									//Clear all particles
									comp.p_system->particles.clear();
									};
								// Store the undo action
								change_render_type.undo_action = [&, render_type = previous_render_type]() {
									comp.render_type = render_type;

									//Clear all particles
									comp.p_system->particles.clear();
									};
								// Execute the action
								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_render_type));
							}
						}
					}

					ImGui::BeginDisabled(comp.render_type != static_cast<int>(NIKE::SysParticle::Data::ParticleRenderType::TEXTURED));

					// For texture id
					{
						// Hold the current and previous texture selection
						static std::string previous_texture = comp.texture_ref;
						std::string current_texture = comp.texture_ref;

						ImGui::Text("Select Texture");

						auto const& all_loaded_textures = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Texture);

						// Find the index of the currently selected texture in the list
						int current_index = -1;
						for (size_t i = 0; i < all_loaded_textures.size(); ++i) {
							if (current_texture == all_loaded_textures[i]) {
								current_index = static_cast<int>(i);
								break;
							}
						}

						// Display combo box for texture selection
						if (ImGui::Combo("##SelectTexture", &current_index, all_loaded_textures.data(), static_cast<int>(all_loaded_textures.size()))) {

							// Validate the selected index and get the new texture
							if (current_index >= 0 && current_index < static_cast<int>(all_loaded_textures.size())) {
								std::string new_texture = all_loaded_textures[current_index];

								if (new_texture != comp.texture_ref) {
									// Save action
									LevelEditor::Action change_font_action;
									change_font_action.do_action = [&, texture_ref = new_texture]() {
										comp.texture_ref = texture_ref;
										};

									// Undo action
									change_font_action.undo_action = [&, texture_ref = previous_texture]() {
										comp.texture_ref = texture_ref;
										};

									// Execute the action
									NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_font_action));

									// Update the previous texture
									previous_texture = new_texture;
								}
							}
						}
						if (ImGui::IsItemHovered()) {
							ImGui::SetTooltip("Select a texture or drag & drop a texture file.");
						}
						if (ImGui::BeginDragDropTarget()) {
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Texture_FILE")) {
								const char* dropped_file = static_cast<const char*>(payload->Data);
								if (dropped_file) {
									std::string new_texture = dropped_file;

									// Ensure it's a valid texture format

									LevelEditor::Action change_texture_action;
									change_texture_action.do_action = [&, texture_ref = new_texture]() {
										comp.texture_ref = texture_ref;
										};

									change_texture_action.undo_action = [&, texture_ref = previous_texture]() {
										comp.texture_ref = texture_ref;
										};

									NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_texture_action));
									previous_texture = new_texture;

								}
							}
							ImGui::EndDragDropTarget();
						}
					}

					ImGui::EndDisabled();

					// !TODO: jspoh add fadeout option, considering color change over time, and texture fadeout. but only if required lol

					// Offset
					ImGui::Text("Particle Offset:");
					ImGui::DragFloat2("##Offset", &comp.offset.x, 0.1f, -1000.f, 1000.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
					//Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_offset = comp.offset;
					}
					//Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_offset;
						//Change pos do action
						change_offset.do_action = [&, offset = comp.offset]() {
							comp.offset = offset;
							};
						//Change pos undo action
						change_offset.undo_action = [&, offset = before_change_offset]() {
							comp.offset = offset;
							};
						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_offset));
					}

					ImGui::BeginDisabled(comp.preset != static_cast<int>(SysParticle::Data::ParticlePresets::NONE));

					// particle settings
					ImGui::Spacing();
					ImGui::Text("Custom particle settings:");
					ImGui::Spacing();

					// Num new particles per second
					{
						ImGui::Text("Num New Particles Per Second:");
						ImGui::DragInt("##Num New Particles Per Second", &comp.num_new_particles_per_second, 1, 0, 20, "%d", ImGuiSliderFlags_AlwaysClamp);
						if (ImGui::IsItemActivated()) {
							before_change_num_new_particles_per_second = comp.num_new_particles_per_second;
						}
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_num_new_particles_per_second;
							change_num_new_particles_per_second.do_action = [&, num_new_particles_per_second = comp.num_new_particles_per_second]() {
								comp.num_new_particles_per_second = num_new_particles_per_second;
								};
							change_num_new_particles_per_second.undo_action = [&, num_new_particles_per_second = before_change_num_new_particles_per_second]() {
								comp.num_new_particles_per_second = num_new_particles_per_second;
								};
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_num_new_particles_per_second));
						}
					}

					// Particle Lifespan
					{
						ImGui::Text("Particle Lifespan:");
						ImGui::DragFloat("##Particle Lifespan", &comp.particle_lifespan, 0.1f, 0.f, 1000.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
						if (ImGui::IsItemActivated()) {
							before_change_particle_lifespan = comp.particle_lifespan;
						}
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_particle_lifespan;
							change_particle_lifespan.do_action = [&, particle_lifespan = comp.particle_lifespan]() {
								comp.particle_lifespan = particle_lifespan;
								};
							change_particle_lifespan.undo_action = [&, particle_lifespan = before_change_particle_lifespan]() {
								comp.particle_lifespan = particle_lifespan;
								};
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_particle_lifespan));
						}
					}

					// Particle Velocity Range
					{
						ImGui::Text("Particle Velocity Range:");
						ImGui::DragFloat2("##Particle Velocity Range", reinterpret_cast<float*>(&comp.particle_velocity_range), 0.1f, 0.f, 1000.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
						if (ImGui::IsItemActivated()) {
							before_change_particle_velocity_range = comp.particle_velocity_range;
						}
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_particle_velocity_range;
							change_particle_velocity_range.do_action = [&, particle_velocity_range = comp.particle_velocity_range]() {
								comp.particle_velocity_range = particle_velocity_range;
								};
							change_particle_velocity_range.undo_action = [&, particle_velocity_range = before_change_particle_velocity_range]() {
								comp.particle_velocity_range = particle_velocity_range;
								};
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_particle_velocity_range));
						}
					}

					// Particle directional vector range
					{
						ImGui::Text("Particle Directional Vector Range:");
						ImGui::DragFloat2("##Particle Vector X Range", reinterpret_cast<float*>(&comp.particle_vector_x_range), 0.1f, -1.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
						if (ImGui::IsItemActivated()) {
							before_change_particle_vector_x_range = comp.particle_vector_x_range;
						}
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_particle_vector_x_range;
							change_particle_vector_x_range.do_action = [&, particle_vector_x_range = comp.particle_vector_x_range]() {
								comp.particle_vector_x_range = particle_vector_x_range;
								};
							change_particle_vector_x_range.undo_action = [&, particle_vector_x_range = before_change_particle_vector_x_range]() {
								comp.particle_vector_x_range = particle_vector_x_range;
								};
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_particle_vector_x_range));
						}

						ImGui::Text("##Particle Vector Y Range");
						ImGui::DragFloat2("##Particle Vector Y Range", reinterpret_cast<float*>(&comp.particle_vector_y_range), 0.1f, -1.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
						if (ImGui::IsItemActivated()) {
							before_change_particle_vector_y_range = comp.particle_vector_y_range;
						}
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_particle_vector_y_range;
							change_particle_vector_y_range.do_action = [&, particle_vector_y_range = comp.particle_vector_y_range]() {
								comp.particle_vector_y_range = particle_vector_y_range;
								};
							change_particle_vector_y_range.undo_action = [&, particle_vector_y_range = before_change_particle_vector_y_range]() {
								comp.particle_vector_y_range = particle_vector_y_range;
								};
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_particle_vector_y_range));
						}
					}

					// Particle Color
					{
						ImGui::Text("Particle color is random:");
						ImGui::Checkbox("##Particle Color Is Random", &comp.particle_color_is_random);
						if (ImGui::IsItemActivated()) {
							before_change_particle_color_is_random = comp.particle_color_is_random;
						}

						ImGui::BeginDisabled(comp.particle_color_is_random);
						{
							static float particle_color[4] = { comp.particle_color.x, comp.particle_color.y, comp.particle_color.z, comp.particle_color.w };
							ImGui::Text("Particle Color:");
							if (ImGui::ColorPicker4("##Particle Color", particle_color, ImGuiColorEditFlags_AlphaBar)) {
								comp.particle_color = { particle_color[0], particle_color[1], particle_color[2], particle_color[3] };
							}
						}
						ImGui::EndDisabled();
					}

					// Particle Color Changes Over Time
					{
						ImGui::Text("Particle color changes over time:");
						ImGui::Checkbox("##Particle color changes over time", &comp.particle_color_changes_over_time);
						if (ImGui::IsItemActivated()) {
							before_particle_color_changes_over_time = comp.particle_color_changes_over_time;
						}

						ImGui::BeginDisabled(!comp.particle_color_changes_over_time);

						// Particle Final Color
						ImGui::Text("Particle Final Color:");
						static float particle_final_color[4] = { comp.particle_final_color.x, comp.particle_final_color.y, comp.particle_final_color.z, comp.particle_final_color.w };
						if (ImGui::ColorPicker4("##Particle Final Color", particle_final_color, ImGuiColorEditFlags_AlphaBar)) {
							comp.particle_final_color = { particle_final_color[0], particle_final_color[1], particle_final_color[2], particle_final_color[3] };
						}

						ImGui::EndDisabled();
					}

					// Particle Rand Offset Range
					{
						ImGui::Text("Particle Rand Offset Range:");
						ImGui::DragFloat2("##Particle Rand X Offset Range", &comp.particle_rand_x_offset_range.x, 0.1f, -1000.f, 1000.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
						if (ImGui::IsItemActivated()) {
							before_change_particle_rand_x_offset_range = comp.particle_rand_x_offset_range;
						}
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_particle_rand_x_offset_range;
							change_particle_rand_x_offset_range.do_action = [&, particle_rand_x_offset_range = comp.particle_rand_x_offset_range]() {
								comp.particle_rand_x_offset_range = particle_rand_x_offset_range;
								};
							change_particle_rand_x_offset_range.undo_action = [&, particle_rand_x_offset_range = before_change_particle_rand_x_offset_range]() {
								comp.particle_rand_x_offset_range = particle_rand_x_offset_range;
								};
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_particle_rand_x_offset_range));
						}

						//ImGui::SameLine();

						ImGui::DragFloat2("##Particle Rand Y Offset Range", &comp.particle_rand_y_offset_range.x, 0.1f, -1000.f, 1000.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
						if (ImGui::IsItemActivated()) {
							before_change_particle_rand_y_offset_range = comp.particle_rand_y_offset_range;
						}
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_particle_rand_y_offset_range;
							change_particle_rand_y_offset_range.do_action = [&, particle_rand_y_offset_range = comp.particle_rand_y_offset_range]() {
								comp.particle_rand_y_offset_range = particle_rand_y_offset_range;
								};
							change_particle_rand_y_offset_range.undo_action = [&, particle_rand_y_offset_range = before_change_particle_rand_y_offset_range]() {
								comp.particle_rand_y_offset_range = particle_rand_y_offset_range;
								};
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_particle_rand_y_offset_range));
						}
					}

					// Particle Rand Width Range
					{
						ImGui::Text("Particle Rand Size Range:");
						ImGui::DragFloat2("##Particle Rand Size Range", &comp.particle_rand_width_range.x, 0.1f, 0.f, 1000.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
						if (ImGui::IsItemActivated()) {
							before_change_particle_rand_width_range = comp.particle_rand_width_range;
						}
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_particle_rand_width_range;
							change_particle_rand_width_range.do_action = [&, particle_rand_width_range = comp.particle_rand_width_range]() {
								comp.particle_rand_width_range = particle_rand_width_range;
								};
							change_particle_rand_width_range.undo_action = [&, particle_rand_width_range = before_change_particle_rand_width_range]() {
								comp.particle_rand_width_range = particle_rand_width_range;
								};
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_particle_rand_width_range));
						}
					}

					// Particle Rotation
					{
						ImGui::Text("Particle Rotation:");
						ImGui::DragFloat("##Particle Rotation", &comp.particle_rotation, 0.1f, 0.f, 359.9f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
						if (ImGui::IsItemActivated()) {
							before_change_particle_rotation = comp.particle_rotation;
						}
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_particle_rotation;
							change_particle_rotation.do_action = [&, particle_rotation = comp.particle_rotation]() {
								comp.particle_rotation = particle_rotation;
								};
							change_particle_rotation.undo_action = [&, particle_rotation = before_change_particle_rotation]() {
								comp.particle_rotation = particle_rotation;
								};
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_particle_rotation));
						}
					}

					ImGui::Spacing();
					ImGui::Text("Particle behaviour over time");
					ImGui::Spacing();

					// Particle acceleration
					{
						ImGui::Text("Particle Acceleration:");
						ImGui::DragFloat("##Particle Acceleration", &comp.particle_acceleration, 0.1f, -1000.f, 1000.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
						if (ImGui::IsItemActivated()) {
							before_change_particle_acceleration = comp.particle_acceleration;
						}
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_particle_acceleration;
							change_particle_acceleration.do_action = [&, particle_acceleration = comp.particle_acceleration]() {
								comp.particle_acceleration = particle_acceleration;
								};
							change_particle_acceleration.undo_action = [&, particle_acceleration = before_change_particle_acceleration]() {
								comp.particle_acceleration = particle_acceleration;
								};
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_particle_acceleration));
						}
					}

					// particle size changes over time
					{
						ImGui::Text("Particle size changes over time:");
						ImGui::Checkbox("##Particle size changes over time", &comp.particle_size_changes_over_time);
						if (ImGui::IsItemActivated()) {
							before_change_particle_final_size_changes_over_time = comp.particle_size_changes_over_time;
						}

						ImGui::BeginDisabled(!comp.particle_size_changes_over_time);

						// Particle Final Size
						ImGui::Text("Particle Final Size:");
						ImGui::DragFloat2("##Particle Final Size", reinterpret_cast<float*>(& comp.particle_final_size), 0.1f, 0.f, 1000.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
						if (ImGui::IsItemActivated()) {
							before_change_particle_final_size = comp.particle_final_size.x;
						}
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_particle_final_size;
							change_particle_final_size.do_action = [&, particle_final_size = comp.particle_final_size]() {
								comp.particle_final_size = particle_final_size;
								};
							change_particle_final_size.undo_action = [&, particle_final_size = before_change_particle_final_size]() {
								comp.particle_final_size.x = particle_final_size;
								comp.particle_final_size.y = particle_final_size;
								};
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_particle_final_size));
						}

						ImGui::EndDisabled();
					}

					// Particle rotation speed
					ImGui::Text("Particle Rotation Speed(degrees anticlockwise / second):");
					ImGui::DragFloat("##Particle Rotation Speed", &comp.particle_rotation_speed, 0.1f, 0.f, 1000.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
					if (ImGui::IsItemActivated()) {
						before_change_particle_rotation_speed = comp.particle_rotation_speed;
					}
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_particle_rotation_speed;
						change_particle_rotation_speed.do_action = [&, particle_rotation_speed = comp.particle_rotation_speed]() {
							comp.particle_rotation_speed = particle_rotation_speed;
							};
						change_particle_rotation_speed.undo_action = [&, particle_rotation_speed = before_change_particle_rotation_speed]() {
							comp.particle_rotation_speed = particle_rotation_speed;
							};
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_particle_rotation_speed));
					}

					ImGui::EndDisabled();		// end custom particle settings

					// advanced options (for programmatic usage only)

					ImGui::BeginDisabled(true);
					// Duration
					static bool infinite = comp.duration == -1.f;
					static bool prev_infinite = infinite;
					ImGui::Checkbox("Infinite Duration", &infinite);
					if (infinite) {
						comp.duration = -1.f;
					}
					else if (prev_infinite && !infinite) {
						comp.duration = 0.f;
					}
					prev_infinite = infinite;

					//ImGui::BeginDisabled(infinite);
					ImGui::Text("Particle Duration:");
					ImGui::DragFloat("##Duration", &comp.duration, 0.1f, 0.f, 1000.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
					ImGui::EndDisabled();
				}
			}
		);

		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Render::Cam>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Render::Cam& comp) {
				ImGui::Text("Edit Camera variables");

				// For cam height
				{
					//Position before change
					static float before_change;

					ImGui::DragFloat("Camera Zoom", &comp.zoom, 0.1f, 0.f, float(UINT16_MAX), "%.3f", ImGuiSliderFlags_AlwaysClamp);

					//Check if position has begun editing
					if (ImGui::IsItemActivated()) {
						before_change = comp.zoom;
					}

					//Check if position has finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_zoom;

						//Change pos do action
						change_zoom.do_action = [&, zoom = comp.zoom]() {
							comp.zoom = zoom;
							};

						//Change pos undo action
						change_zoom.undo_action = [&, zoom = before_change]() {
							comp.zoom = zoom;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_zoom));
					}

				}

				// For cam offset
				{
					//Position before change
					static float before_move_offset;

					ImGui::DragFloat("Camera move offset", &comp.mouse_offset, 0.005f, 0.f, float(UINT16_MAX), "%.3f", ImGuiSliderFlags_AlwaysClamp);

					//Check if position has begun editing
					if (ImGui::IsItemActivated()) {
						before_move_offset = comp.mouse_offset;
					}

					//Check if position has finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_mouse_offset;

						//Change pos do action
						change_mouse_offset.do_action = [&, mouse_offset = comp.mouse_offset]() {
							comp.mouse_offset = mouse_offset;
							};

						//Change pos undo action
						change_mouse_offset.undo_action = [&, mouse_offset = before_move_offset]() {
							comp.mouse_offset = mouse_offset;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_mouse_offset));
					}

				}
			}
		);

		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Render::Text>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Render::Text& comp) {

				//Static variables for string input management
				static std::string text_input;

				//Initialization of string inputs upon collapsible shown
				if (ImGui::IsItemActivated() || comp_panel.isEntityChanged()) {
					text_input = comp.text;
				}

				ImGui::Text("Edit Text variables");

				ImGui::Spacing();

				// For Text Scale
				{
					// Before change
					static float editing_initial_scale = 0.0f;

					// Allow scale adjustment UI
					ImGui::DragFloat("Text Scale", &comp.scale, 0.1f, 0.f, 10.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

					//Check if begin editing
					if (ImGui::IsItemActivated()) {
						editing_initial_scale = comp.scale;
					}

					//Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_text_scale;

						//Change pos do action
						change_text_scale.do_action = [&, scale = comp.scale]() {
							comp.scale = scale;
							};

						//Change pos undo action
						change_text_scale.undo_action = [&, scale = editing_initial_scale]() {
							comp.scale = scale;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_text_scale));
					}

				}

				ImGui::Spacing();

				//ImGui::Text((std::string("Text Size X: ") + std::to_string(comp.size.x)).c_str());
				//ImGui::Text((std::string("Text Size Y: ") + std::to_string(comp.size.y)).c_str());

				// For Text color
				{
					// Before change
					static Vector4f before_change;

					ImGui::Text("Adjust text color:");

					ImGui::ColorPicker4("Text Color", &comp.color.x, ImGuiColorEditFlags_AlphaBar);

					//Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change = comp.color;
					}

					//Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_color;

						//Change pos do action
						change_color.do_action = [&, color = comp.color]() {
							comp.color = color;
							};

						//Change pos undo action
						change_color.undo_action = [&, color = before_change]() {
							comp.color = color;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_color));
					}

				}

				ImGui::Spacing();

				// For Text font
				{
					// Display a combo box for selecting a font
					ImGui::Text("Select Font:");

					// Hold the current and previous font selection
					static std::string previous_font_id = comp.font_id;
					std::string current_font_id = comp.font_id;

					// Get all loaded fonts
					const auto& all_loaded_fonts = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Font);

					// Find the index of the currently selected font in the list
					int current_index = -1;
					for (size_t i = 0; i < all_loaded_fonts.size(); ++i) {
						if (current_font_id == all_loaded_fonts[i]) {
							current_index = static_cast<int>(i);
							break;
						}
					}

					// Display combo box for font selection
					if (ImGui::Combo("##SelectFont", &current_index, all_loaded_fonts.data(), static_cast<int>(all_loaded_fonts.size()))) {
						// Validate the selected index and get the new font ID
						if (current_index >= 0 && current_index < static_cast<int>(all_loaded_fonts.size())) {
							std::string new_font_id = all_loaded_fonts[current_index];
							if (new_font_id != comp.font_id) {
								// Save action
								LevelEditor::Action change_font_action;
								change_font_action.do_action = [&, font_id = new_font_id]() {
									comp.font_id = font_id;
									};

								// Undo action
								change_font_action.undo_action = [&, font_id = previous_font_id]() {
									comp.font_id = font_id;
									};

								// Execute the action
								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_font_action));

								// Update the previous font ID
								previous_font_id = new_font_id;
							}
						}
					}
				}

				//Set Text input
				if (!comp.font_id.empty())
				{
					{
						//Set Text input
						ImGui::Text("Enter Text:");
						if (ImGui::InputText("##TextInput", text_input.data(), text_input.capacity() + 10)) {
							text_input.resize(strlen(text_input.c_str()));
						}

						ImGui::SameLine();

						//Save text input Button
						if (ImGui::Button("Save##TextInput")) {
							LevelEditor::Action save_text;

							//Save texrt action
							save_text.do_action = [&, text = text_input]() {
								comp.text = text;
								text_input = comp.text;
								};

							//Undo save text action
							save_text.undo_action = [&, text = comp.text]() {
								comp.text = text;
								text_input = comp.text;
								};

							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_text));
							comp_panel.setPopUpSuccessMsg("Text Saved successfully");
							comp_panel.openPopUp("Success");
						}
					}

					ImGui::Spacing();

					// For Text Origin
					{
						ImGui::Text("Adjust text origin:");
						static const char* origin_names[] = { "CENTER", "TOP", "BOTTOM", "RIGHT", "LEFT" };
						// Hold the current selection and the previous value
						static NIKE::Render::TextOrigin before_select_origin;
						static int previous_origin = static_cast<int>(comp.origin);
						int current_origin = static_cast<int>(comp.origin);
						// Combo returns one bool check
						if (ImGui::Combo("##TextOrigin", &current_origin, origin_names, IM_ARRAYSIZE(origin_names))) {
							NIKE::Render::TextOrigin new_origin = static_cast<NIKE::Render::TextOrigin>(current_origin);
							if (new_origin != comp.origin) {
								// Save action
								LevelEditor::Action save_text;
								save_text.do_action = [&, origin = new_origin]() {
									comp.origin = origin;
									};

								// Undo action
								save_text.undo_action = [&, origin = before_select_origin]() {
									comp.origin = origin;
									};

								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_text));

								// Update the previous value
								before_select_origin = comp.origin;
								// Apply the new origin
								comp.origin = new_origin;
							}
						}
					}
				}

			}
		);

		// UI for shape
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Render::Shape>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Render::Shape& comp) {
				ImGui::Text("Edit Shape variables");

				ImGui::Spacing();

				// For shape model
				{
					// Display a combo box for selecting a model
					ImGui::Text("Select Shape:");

					// Hold the current and previous model selection
					static std::string previous_model_id = comp.model_id;
					std::string current_model_id = comp.model_id;

					// Get all loaded models
					const auto& all_loaded_models = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Model);

					// Find the index of the currently selected model in the list
					int current_index = -1;
					for (size_t i = 0; i < all_loaded_models.size(); ++i) {
						if (current_model_id == all_loaded_models[i]) {
							current_index = static_cast<int>(i);
							break;
						}
					}

					// Display combo box for model selection
					if (ImGui::Combo("##SelectModel", &current_index, all_loaded_models.data(), static_cast<int>(all_loaded_models.size()))) {
						// Validate the selected index and get the new model ID
						if (current_index >= 0 && current_index < static_cast<int>(all_loaded_models.size())) {
							std::string new_model_id = all_loaded_models[current_index];
							if (new_model_id != comp.model_id) {
								// Save action
								LevelEditor::Action change_model_action;
								change_model_action.do_action = [&, model_id = new_model_id]() {
									comp.model_id = model_id;
									};

								// Undo action
								change_model_action.undo_action = [&, model_id = previous_model_id]() {
									comp.model_id = model_id;
									};

								// Execute the action
								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_model_action));

								// Update the previous model ID
								previous_model_id = new_model_id;
							}
						}
					}
				}

				ImGui::Spacing();

				// For shape color
				if (!comp.model_id.empty())
				{
					{
						// Before change
						static Vector4f before_change;

						ImGui::Text("Adjust shape color:");

						ImGui::ColorPicker4("Shape Color", &comp.color.x, ImGuiColorEditFlags_AlphaBar);

						//Check if begin editing
						if (ImGui::IsItemActivated()) {
							before_change = comp.color;
						}

						//Check if finished editing
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_shape_color;

							//Change pos do action
							change_shape_color.do_action = [&, color = comp.color]() {
								comp.color = color;
								};

							//Change pos undo action
							change_shape_color.undo_action = [&, color = before_change]() {
								comp.color = color;
								};

							//Execute action
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_shape_color));
						}


					}

				}
			}
		);

		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Render::Texture>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Render::Texture& comp) {

				ImGui::Text("Edit Texture variables");

				ImGui::Spacing();

				// For texture id
				{
					// Hold the current and previous texture selection
					static std::string previous_texture = comp.texture_id;
					std::string current_texture = comp.texture_id;

					ImGui::Text("Select Texture");

					auto const& all_loaded_textures = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Texture);

					// Find the index of the currently selected texture in the list
					int current_index = -1;
					for (size_t i = 0; i < all_loaded_textures.size(); ++i) {
						if (current_texture == all_loaded_textures[i]) {
							current_index = static_cast<int>(i);
							break;
						}
					}

					// Display combo box for texture selection
					if (ImGui::Combo("##SelectTexture", &current_index, all_loaded_textures.data(), static_cast<int>(all_loaded_textures.size()))) {

						// Validate the selected index and get the new texture
						if (current_index >= 0 && current_index < static_cast<int>(all_loaded_textures.size())) {
							std::string new_texture = all_loaded_textures[current_index];

							if (new_texture != comp.texture_id) {
								// Save action
								LevelEditor::Action change_font_action;
								change_font_action.do_action = [&, texture_id = new_texture]() {
									comp.texture_id = texture_id;
									};

								// Undo action
								change_font_action.undo_action = [&, texture_id = previous_texture]() {
									comp.texture_id = texture_id;
									};

								// Execute the action
								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_font_action));

								// Update the previous texture
								previous_texture = new_texture;
							}
						}
					}
					if (ImGui::IsItemHovered()) {
						ImGui::SetTooltip("Select a texture or drag & drop a texture file.");
					}
					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Texture_FILE")) {
							const char* dropped_file = static_cast<const char*>(payload->Data);
							if (dropped_file) {
								std::string new_texture = dropped_file;

								// Ensure it's a valid texture format
								
									LevelEditor::Action change_texture_action;
									change_texture_action.do_action = [&, texture_id = new_texture]() {
										comp.texture_id = texture_id;
										};

									change_texture_action.undo_action = [&, texture_id = previous_texture]() {
										comp.texture_id = texture_id;
										};

									NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_texture_action));
									previous_texture = new_texture;
								
							}
						}
						ImGui::EndDragDropTarget();
					}
				}

				ImGui::Spacing();

				if (!comp.texture_id.empty())
				{
					// For texture frame size
					{
						//Before change
						static Vector2i frame_size_before_change;

						//Change frame size
						if (ImGui::DragInt2("Frame Size", &comp.frame_size.x, 1, 1, 100)) {
							// Ensure frame size is never zero by clamping the values
							comp.frame_size.x = max(comp.frame_size.x, 1);
							comp.frame_size.y = max(comp.frame_size.y, 1);
						}

						//Check if begun editing
						if (ImGui::IsItemActivated()) {
							frame_size_before_change = comp.frame_size;
						}

						//Check if finished editing
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_frame_size;

							//Change do action
							change_frame_size.do_action = [&, frame_size = comp.frame_size]() {
								comp.frame_size = frame_size;
								};

							//Change undo action
							change_frame_size.undo_action = [&, frame_size = frame_size_before_change]() {
								comp.frame_size = frame_size;
								};

							//Execute action
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_frame_size));
						}
					}

					ImGui::Spacing();

					// For texture frame index
					{
						// Before change
						static Vector2i frame_index_before_change;

						//Change frame index
						if (ImGui::DragInt2("Frame Index", &comp.frame_index.x, 1, 0, 100))
						{
							comp.frame_index.x = std::clamp(comp.frame_index.x, 0, comp.frame_size.x);
							comp.frame_index.y = std::clamp(comp.frame_index.y, 0, comp.frame_size.y);
						}

						//Check if begun editing
						if (ImGui::IsItemActivated()) {
							frame_index_before_change = comp.frame_index;
						}

						//Check if finished editing
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_frame_index;

							//Change do action
							change_frame_index.do_action = [&, frame_index = comp.frame_index]() {
								comp.frame_index = frame_index;
								};

							//Change undo action
							change_frame_index.undo_action = [&, frame_index = frame_index_before_change]() {
								comp.frame_index = frame_index;
								};

							//Execute action
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_frame_index));
						}
					}

					ImGui::Spacing();

					// For texture color
					{
						// Before change
						static Vector4f texture_color_before_change;

						ImGui::Text("Adjust texture blend color:");

						ImGui::ColorPicker4("Texture Color", &comp.color.x, ImGuiColorEditFlags_AlphaBar);


						//Check if has begun editing
						if (ImGui::IsItemActivated()) {
							texture_color_before_change = comp.color;
						}

						//Check if  finished editing
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_color;

							//Change do action
							change_color.do_action = [&, color = comp.color]() {
								comp.color = color;
								};

							//Change undo action
							change_color.undo_action = [&, color = texture_color_before_change]() {
								comp.color = color;
								};

							//Execute action
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_color));
						}
					}

					ImGui::Spacing();

					// For texture intensity
					{
						// Before change
						static float intensity_bef_change;

						ImGui::DragFloat("Texture Intensity", &comp.intensity, 0.001f, 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);


						//Check if has begun editing
						if (ImGui::IsItemActivated()) {
							intensity_bef_change = comp.intensity;
						}

						//Check if  finished editing
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_intensity;

							//Change do action
							change_intensity.do_action = [&, intensity = comp.intensity]() {
								comp.intensity = intensity;
								};

							//Change undo action
							change_intensity.undo_action = [&, intensity = intensity_bef_change]() {
								comp.intensity = intensity;
								};

							//Execute action
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_intensity));
						}
					}

					ImGui::Spacing();

					// Stretch checkbox
					{
						static bool before_bool_stretch = comp.b_stretch;
						if (ImGui::Checkbox("Stretch", &comp.b_stretch))
						{
							if (comp.b_stretch != before_bool_stretch)
							{
								// Save action
								LevelEditor::Action change_stretch;
								change_stretch.do_action = [&, stretch = comp.b_stretch]() {
									comp.b_stretch = stretch;
									};

								// Undo action
								change_stretch.undo_action = [&, stretch = before_bool_stretch]() {
									comp.b_stretch = stretch;
									};

								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_stretch));

								// Update the previous value
								before_bool_stretch = comp.b_stretch;
							}

						}
					}

					ImGui::Spacing();

					// Blend checkbox
					{
						static bool before_bool_blend = comp.b_blend;
						if (ImGui::Checkbox("Blend", &comp.b_blend))
						{
							if (comp.b_blend != before_bool_blend)
							{
								// Save action
								LevelEditor::Action change_blend;
								change_blend.do_action = [&, blend = comp.b_blend]() {
									comp.b_blend = blend;
									};

								// Undo action
								change_blend.undo_action = [&, blend = before_bool_blend]() {
									comp.b_blend = blend;
									};

								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_blend));

								// Update the previous value
								before_bool_blend = comp.b_blend;
							}

						}
					}

					ImGui::Spacing();

					// Flip x-axis checkbox
					{
						static bool before_bool_flip_x = comp.b_flip.x;
						if (ImGui::Checkbox("Flip Horizontally (x-axis) ", &comp.b_flip.x))
						{
							if (comp.b_flip.x != before_bool_flip_x)
							{
								// Save action
								LevelEditor::Action change_flip_x;
								change_flip_x.do_action = [&, flip_x = comp.b_flip.x]() {
									comp.b_flip.x = flip_x;
									};

								// Undo action
								change_flip_x.undo_action = [&, flip_x = before_bool_flip_x]() {
									comp.b_flip.x = flip_x;
									};

								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_flip_x));

								// Update the previous value
								before_bool_flip_x = comp.b_flip.x;
							}

						}
					}

					ImGui::Spacing();

					// Flip y-axis checkbox
					{
						static bool before_bool_flip_y = comp.b_flip.y;
						if (ImGui::Checkbox("Flip Horizontally (y-axis) ", &comp.b_flip.y))
						{
							if (comp.b_flip.y != before_bool_flip_y)
							{
								// Save action
								LevelEditor::Action change_flip_y;
								change_flip_y.do_action = [&, flip_y = comp.b_flip.y]() {
									comp.b_flip.y = flip_y;
									};

								// Undo action
								change_flip_y.undo_action = [&, flip_y = before_bool_flip_y]() {
									comp.b_flip.y = flip_y;
									};

								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_flip_y));

								// Update the previous value
								before_bool_flip_y = comp.b_flip.y;
							}

						}
					}

				}

			}
		);

		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Render::Video>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Render::Video& comp) {

				ImGui::Text("Edit Video variables");

				ImGui::Spacing();

				// For Video id
				{
					// Hold the current and previous video selection
					static std::string prev_video = comp.video_id;
					std::string current_video = comp.video_id;

					ImGui::Text("Select Video");

					auto const& all_loaded_video = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Video);

					// Find the index of the currently selected video in the list
					int current_index = -1;
					for (size_t i = 0; i < all_loaded_video.size(); ++i) {
						if (current_video == all_loaded_video[i]) {
							current_index = static_cast<int>(i);
							break;
						}
					}

					// Display combo box for video selection
					if (ImGui::Combo("##SelectVideo", &current_index, all_loaded_video.data(), static_cast<int>(all_loaded_video.size()))) {

						// Validate the selected index and get the new video
						if (current_index >= 0 && current_index < static_cast<int>(all_loaded_video.size())) {
							std::string new_video = all_loaded_video[current_index];

							if (new_video != comp.video_id) {
								// Save action
								LevelEditor::Action change_video_action;
								change_video_action.do_action = [&, video_id = new_video]() {
									comp.video_id = video_id;
									comp.b_init = true;
									};

								// Undo action
								change_video_action.undo_action = [&, video_id = prev_video]() {
									comp.video_id = video_id;
									comp.b_init = true;
									};

								// Execute the action
								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_video_action));

								// Update the previous texture
								prev_video = new_video;
							}
						}
					}

					if (ImGui::IsItemHovered()) {
						ImGui::SetTooltip("Select a video or drag & drop a video file.");
					}

					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Video_FILE")) {
							const char* dropped_file = static_cast<const char*>(payload->Data);
							if (dropped_file) {
								std::string new_video = dropped_file;

								// Save action
								LevelEditor::Action change_video_action;
								change_video_action.do_action = [&, video_id = new_video]() {
									comp.video_id = video_id;
									comp.b_init = true;
									};

								change_video_action.undo_action = [&, video_id = prev_video]() {
									comp.video_id = video_id;
									comp.b_init = true;
									};

								// Execute the action
								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_video_action));

								// Update the previous texture
								prev_video = new_video;

							}
						}
						ImGui::EndDragDropTarget();
					}
				}

				//To control video
				{
					ImGui::Text("Video Duration: %.3f secs / %.3f secs", comp.curr_time, comp.duration);

					ImGui::Text("Video Playing: %s", comp.b_is_playing ? "True" : "False");

					ImGui::Text("Video Looping:");

					ImGui::SameLine();

					if (ImGui::SmallButton(comp.b_loop ? "True" : "False")) {
						comp.b_loop = !comp.b_loop;
					}
				}

			}
		);
#endif
	}
}