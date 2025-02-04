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

namespace NIKE {
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

			Cam() : position(), zoom{ 1.0f } {}
			Cam(float zoom) : position(), zoom{ zoom } {}
			Cam(Vector2f const& position, float zoom) : position{ position }, zoom{ zoom } {}
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

		struct Hidden {
		};

		/**
		 * built in components will not be saved to scene files.
		 */
		struct BuiltIn {
		};

		void registerComponents();

		void registerEditorComponents();
	}
}

#endif // !C_RENDER_HPP
