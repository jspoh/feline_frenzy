/*****************************************************************//**
 * \file   cRender.h
 * \brief
 *
 * \author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef C_RENDER_HPP
#define C_RENDER_HPP

#include "Core/stdafx.h"
#include "Managers/Services/sEvents.h"
#include "Managers/Services/sAudio.h"

namespace NIKE {

	namespace SysParticle {
		struct ParticleSystem;
		class Data;
		class Manager;
	}

	namespace Render {
		enum class CamPosition {
			UP = 0,
			DOWN,
			LEFT,
			RIGHT,
			RESET_POS,
			NONE
		};

		enum class CamZoom {
			ZOOM_IN = 0,
			ZOOM_OUT,
			RESET_ZOOM,
			NONE
		};

		struct Cam {
			Vector2f position;	// Position of camera
			float zoom;	// represents how much of the world is visible vertically (zoom level).
			float mouse_offset;

			Cam() : position(), zoom{ 1.0f }, mouse_offset{ 0.f } {}
			Cam(float zoom) : position(), zoom{ zoom }, mouse_offset{ 0.f } {}
			Cam(Vector2f const& position, float zoom, float mouse_offset = 0.f) : position{ position }, zoom{ zoom }, mouse_offset{ mouse_offset } {}
		};

		//Change camera event
		struct ChangeCamEvent : public Events::IEvent {
			std::string entity_name;
			std::shared_ptr<Render::Cam> fallback_cam;

			//If a entity id does not have a camera attached, default camera is deployed
			ChangeCamEvent(std::string const& entity_name, std::shared_ptr<Render::Cam> fallback_cam = nullptr)
				: entity_name{ entity_name }, fallback_cam{ fallback_cam } {}
		};

		enum class TextOrigin {
			CENTER = 0,
			TOP,
			BOTTOM,
			RIGHT,
			LEFT
		};

		struct Text {
			std::string font_id;
			std::string text;
			Vector4f color;
			float scale;
			Vector2f size;
			TextOrigin origin;

			Text() : font_id{ "" }, text{ "" }, color{0.0f, 0.0f, 0.0f, 1.0f}, scale{ 1.0f }, size(), origin{ TextOrigin::CENTER } {}
			Text(std::string const& font_id, std::string const& text, Vector4f const& color, float scale, TextOrigin origin = TextOrigin::CENTER)
				: font_id{ font_id }, text{ text }, color{ color }, scale{ scale }, size(), origin{ origin } {}
		};

		// set use_override_color to true if setting an override_color
		struct Shape {
			std::string model_id;
			Vector2f pos;

			Vector4f color;
			bool use_override_color;

			Shape() :model_id{}, color(1.f, 1.f, 1.f, 1.f), pos{}, use_override_color { false } {}
			Shape(std::string const& model_id, Vector2f const& pos) : model_id{ model_id }, pos{ pos }, color(), use_override_color{ false } {};
			Shape(const std::string& model_id, const Vector4f& color, const Vector2f& pos)
				: model_id{ model_id }, pos{ pos }, color{ color }, use_override_color{ true } {}
			Shape(std::string const& model_id, Vector4f const& color) : model_id{ model_id }, color{ color }, pos{}, use_override_color{ true } {};
		};

		struct Texture {
			std::string texture_id;
			Vector4f color;
			Vector2i frame_size;	// x: 1 / frames in col,  y: 1 / frames in row
			Vector2i frame_index;	// frame 1: (0,0), frame 2: (1,0) ( topleft to bot right )
			bool b_blend;
			float intensity;
			bool b_stretch;
			Vector2b b_flip;

			Texture() : texture_id{}, color(0.f,0.f,0.f,1.f), b_blend{ false }, intensity{ 0.0f }, b_stretch{ false }, frame_size(1,1), frame_index(0,0), b_flip{false, false} {}
			Texture(std::string const& texture_id, Vector4f const& color, bool b_blend = false, float intensity = 0.5f, bool b_stretch = false, Vector2i const& frame_size = { 1, 1 }, Vector2i const& frame_index = { 0, 0 }, Vector2b const& b_flip = {false, false})
				:texture_id{ texture_id }, color{ color }, b_blend{ b_blend }, intensity{ intensity }, b_stretch{ b_stretch }, frame_size{ frame_size }, frame_index{ frame_index }, b_flip{ b_flip } {}
		};

		struct ParticleEmitter {
			Vector2f offset{};		// offset from entity position
			int render_type{};	// ParticleRenderType type
			int preset{};			// ParticlePresets type
			std::string ref{};		// reference to particle system
			float duration{};		// -1 for infinite

			//Particle system
			std::shared_ptr<SysParticle::ParticleSystem> p_system;

			// particle
			int num_new_particles_per_second{10};
			float particle_lifespan{1.f};
			float particle_acceleration{};
			Vector2f particle_velocity_range{5.f, 100.f};
			Vector2f particle_vector_x_range{-1.f, 1.f};
			Vector2f particle_vector_y_range{-1.f, 1.f};
			bool particle_color_is_random{true};
			Vector4f particle_color{1.f, 1.f, 1.f, 1.f};
			Vector2f particle_rand_x_offset_range{0.f, 0.f};
			Vector2f particle_rand_y_offset_range{0.f, 0.f};
			float particle_rotation{};
			Vector2f particle_rand_width_range{5.f, 10.f};

			// particle behaviour over time
			bool particle_size_changes_over_time{false};
			Vector2f particle_final_size{};
			bool particle_color_changes_over_time{false};
			Vector4f particle_final_color{};
			float particle_rotation_speed{};		// in degrees (anticlockwise) per second !NOTE: rmb to convert to radians lol
			
			// texture
			std::string texture_ref{};

			ParticleEmitter();
		};
		struct Video {

			//Video ID
			std::string video_id;

			//Mpeg object
			plm_t* mpeg;
			uint8_t* rgb_data;
			bool b_init;

			//Texture ID
			unsigned int texture_id;
			Vector2f texture_size;

			//Audio
			std::shared_ptr<Audio::IAudio> audio;
			std::shared_ptr<Audio::IChannel> channel;
			static std::shared_ptr<Audio::IChannelGroup> channel_group;

			//Video variables
			bool b_is_playing;
			bool b_loop;
			float curr_time;
			float duration;

			//Timers
			float frame_timer;
			float audio_timer;

			Video() : video_id{ "" }, mpeg{ nullptr }, rgb_data{ nullptr }, b_init{ true }, texture_id{ 0 }, texture_size(),
				b_is_playing{ false }, b_loop{ true }, curr_time{ 0.0f }, duration{ 0.0f }, 
				frame_timer{ 0.0f }, audio_timer{ 0.0f } {}
		};

		void registerComponents();

		void registerEditorComponents();
	}
}

#endif // !C_RENDER_HPP
