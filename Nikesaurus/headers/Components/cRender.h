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

		struct Color{
			float r, g, b, a;
			
			Color() : r{ 0.0f }, g{ 0.0f }, b{ 0.0f }, a{ 0.0f } {}
			Color(float r, float g, float b, float a) : r{ r }, g{ g }, b{ b }, a{ a } {}
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
			Color color;
			float scale;
			TextOrigin origin;

			Text() : font_ref{ "" }, text{ "" }, color(), scale{ 1.0f }, origin{ TextOrigin::CENTER } {}
			Text(std::string const& font_ref, std::string const& text, Color const& color, float scale, TextOrigin origin = TextOrigin::CENTER)
				: font_ref{ font_ref }, text{ text }, color{ color }, scale{ scale }, origin{ origin } {}
		};

		struct Shape {
			std::string model_ref; 
			Color color;

			Shape() : model_ref{ "" }, color() {}
			Shape(std::string const& model_ref, Color const& color) : model_ref{ model_ref }, color{ color } {};
		};

		struct Texture {
			std::string texture_ref;
			Color color;
			Vector2f frame_size;	// x: 1 / frames in col,  y: 1 / frames in row
			Vector2i frame_index;	// frame 1: (0,0), frame 2: (1,0) ( topleft to bot right )
			bool b_stretch;

			Texture() : texture_ref{ "" }, color(), frame_size(), frame_index(), b_stretch{ false } {}
			Texture(std::string const& texture_ref, Color const& color, bool b_stretch = false, Vector2f const& frame_size = {1.0f, 1.0f}, Vector2i const& frame_index = {0, 0})
				:texture_ref{ texture_ref }, color{ color }, frame_size{ frame_size }, frame_index{ frame_index }, b_stretch{ b_stretch } {}
		};
	}
}

#endif // !C_RENDER_HPP
