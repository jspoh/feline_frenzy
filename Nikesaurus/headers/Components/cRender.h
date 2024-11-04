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
			float height;	// represents how much of the world is visible vertically (zoom level).

			Cam() : position(), height{ 0.0f } {}
			Cam(float height) : position(), height{height} {}
			Cam(Vector2f const& position, float height) : position{ position }, height{ height } {}
		};

		//Change camera event
		struct ChangeCamEvent : public Events::IEvent {
			Entity::Type entity_id;

			//If a entity id does not have a camera attached, default camera is deployed
			ChangeCamEvent(Entity::Type entity_id)
				: entity_id{ entity_id } {}
		};

		//Update camera event
		struct UpdateCamEvent : public Events::IEvent {
			std::optional<CamPosition> edit_position; // Optional position change
			std::optional<CamZoom> edit_zoom;      // Optional zoom level change

			UpdateCamEvent(
				std::optional<CamPosition> pos = std::nullopt,
				std::optional<CamZoom> zoom = std::nullopt)
				: edit_position{ pos }, edit_zoom { zoom } {}
		};

		enum class TextOrigin {
			CENTER = 0,
			BOTTOM,
			TOP,
			LEFT,
			RIGHT
		};

		struct Text {
			std::string font_ref;
			std::string text;
			Vector4f color;
			float scale;
			TextOrigin origin;

			Text() : font_ref{ "" }, text{ "" }, color(), scale{ 1.0f }, origin{ TextOrigin::CENTER } {}
			Text(std::string const& font_ref, std::string const& text, Vector4f const& color, float scale, TextOrigin origin = TextOrigin::CENTER)
				: font_ref{ font_ref }, text{ text }, color{ color }, scale{ scale }, origin{ origin } {}
		};

		struct Shape {
			std::string model_ref; 
			Vector4f color;
			Vector2f pos;

			Shape() : model_ref{ "" }, color(), pos() {}
			Shape(std::string const& model_ref, Vector4f const& color, Vector2f const& pos) : model_ref{ model_ref }, color{ color }, pos{ pos } {};
		};

		struct Texture {
			std::string texture_ref;
			Vector4f color;
			Vector2i frame_size;	// x: 1 / frames in col,  y: 1 / frames in row
			Vector2i frame_index;	// frame 1: (0,0), frame 2: (1,0) ( topleft to bot right )
			bool b_blend;
			float intensity;
			bool b_stretch;
			Vector2b b_flip;

			Texture() : texture_ref{ "" }, color(), b_blend{ false }, intensity{ 0.0f }, b_stretch{ false }, frame_size(), frame_index(), b_flip{false, false} {}
			Texture(std::string const& texture_ref, Vector4f const& color, bool b_blend = false, float intensity = 0.5f, bool b_stretch = false, Vector2i const& frame_size = { 1, 1 }, Vector2i const& frame_index = { 0, 0 }, Vector2b const& b_flip = {false, false})
				:texture_ref{ texture_ref }, color{ color }, b_blend{ b_blend }, intensity{ intensity }, b_stretch{ b_stretch }, frame_size{ frame_size }, frame_index{ frame_index }, b_flip{ b_flip } {}
		};

		void registerComponents();
	}
}

#endif // !C_RENDER_HPP
