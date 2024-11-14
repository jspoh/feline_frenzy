/*****************************************************************//**
 * \file   sEditorPanels.h
 * \brief  Level Editor panel implementation
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 *  All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef EDITOR_PANELS_HPP
#define EDITOR_PANELS_HPP

#include "Components/cRender.h"

namespace NIKE {
	namespace LevelEditor {

		//Panel Interface
		class IPanel {
		private:
		public:
			virtual ~IPanel() = default;

			//Panel Name
			virtual std::string getName() const = 0;

			//Init
			virtual void init() {};

			//Update
			virtual void update() = 0;

			//Render
			virtual void render() = 0;
		};

		//Game Window Panel
		class GameWindowPanel : public IPanel, public Events::IEventListener<Render::ViewportTexture> {
		private:
			//Game texture
			unsigned int texture_id;

			//Mouse position relative to Game Window
			Vector2f relative_mouse_pos;

			//Game window render event
			void onEvent(std::shared_ptr<Render::ViewportTexture> event);

		public:
			GameWindowPanel() = default;
			~GameWindowPanel() = default;

			//Panel Name
			std::string getName() const override {
				return "Game Viewport";
			}

			//Get relative mouse position
			Vector2f getRelativeMousePos() const;

			//Check if mouse is in game window
			bool isMouseInWindow() const;

			//Init
			void init() override;

			//Update
			void update() override;

			//Render
			void render() override;
		};

	}
}

#endif //!EDITOR_PANELS_HPP
