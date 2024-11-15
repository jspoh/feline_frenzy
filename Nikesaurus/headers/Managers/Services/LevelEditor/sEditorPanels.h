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

			//Static boolean for signalling popup showing
			static bool b_popup_showing;

			//Internal panel popup maangement
			struct PopUp {
				bool b_is_open = false;
				std::function<void()> popUpFunction;
			};

			//Map of popups
			std::unordered_map<std::string, PopUp> popups;
			
		public:
			virtual ~IPanel() = default;

			//Panel Name
			virtual std::string getName() const = 0;

			//Init
			virtual void init();

			//Update
			virtual void update() = 0;

			//Render
			virtual void render() = 0;

			//Register new popup
			void registerPopUp(std::string const& popup_id, std::function<void()> popup_func);

			//Open popup
			void openPopUp(std::string const& popup_id);

			//Close popup
			void closePopUp(std::string const& popup_id);

			//Render popup
			void renderPopUps();

			//Check if popup is showing
			static bool checkPopUpShowing();

			//Error Popup
			std::function<void()> errorPopUp(std::string const& error_id, std::string const& error_msg);
		};

		//Main Panel ( Docking )
		class MainPanel : public IPanel {
		private:
			//Panel window flags
			int window_flags;

			//Boolean for activating debug mode
			bool b_debug_mode;

			//Boolean for activating play/stop for game
			bool b_game_state;

			//Set Debug State
			void setDebugState(bool state);

			//Set Game State
			void setGameState(bool state);

		public:
			MainPanel() :window_flags{ 0 }, b_debug_mode { false }, b_game_state{ true } {}
			~MainPanel() = default;

			//Panel Name
			std::string getName() const override {
				return "NIKE Level Editor";
			}

			//Public get debug state
			bool getDebugState() const;

			//Public get game state
			bool getGameState() const;

			//Init
			void init() override;

			//Update
			void update() override;

			//Render
			void render() override;
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
			GameWindowPanel() : texture_id{ 0 } {}
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

		//Class Entity Panel
		class EntitiesPanel : public IPanel {
		private:

			//Set of active entities
			std::set<Entity::Type> entities;

			//Unordered map of entities ref
			std::unordered_map<Entity::Type, std::string> entities_ref;

			//Selected entity
			Entity::Type selected_entity;

			//Create entity popup
			std::function<void()> createEntityPopUp(std::string const& popup_id);

			//Remove Entity popup
			std::function<void()> removeEntityPopUp(std::string const& popup_id);

		public:
			EntitiesPanel() : selected_entity{ 0 } {}
			~EntitiesPanel() = default;

			//Panel Name
			std::string getName() const override {
				return "Entities Management";
			}

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
