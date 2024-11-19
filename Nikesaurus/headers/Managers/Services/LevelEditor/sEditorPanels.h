/*****************************************************************//**
 * \file   sEditorPanels.h
 * \brief  Level Editor panel implementation
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 *  All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
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

			//Edit registered popup
			void editPopUp(std::string const& popup_id, std::function<void()> popup_func);

			//Open popup
			void openPopUp(std::string const& popup_id);

			//Close popup
			void closePopUp(std::string const& popup_id);

			//Render popup
			void renderPopUps();

			//Check if popup is showing
			static bool checkPopUpShowing();

			//Default Popup
			std::function<void()> defPopUp(std::string const& id, std::shared_ptr<std::string> msg);
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

			//Boolean for showing/hide grid
			bool b_grid_state;

			//Set Debug State
			void setDebugState(bool state);

			//Set Game State
			void setGameState(bool state);

			//Set Grid State
			void setGridState(bool state);

		public:
			MainPanel() :window_flags{ 0 }, b_debug_mode{ false }, b_game_state{ true }, b_grid_state{ false } {}
			~MainPanel() = default;

			//Panel Name
			std::string getName() const override {
				return "NIKE Level Editor";
			}

			//Static panel name
			static std::string getStaticName() {
				return "NIKE Level Editor";
			}

			//Public get debug state
			bool getDebugState() const;

			//Public get game state
			bool getGameState() const;

			//Public get grid state
			bool getGridState() const;

			//Init
			void init() override;

			//Update
			void update() override;

			//Render
			void render() override;
		};

		//Forward declaration of the tilemap panel
		class TileMapPanel;

		//Game Window Panel
		class GameWindowPanel : public IPanel, public Events::IEventListener<Render::ViewportTexture> {
		private:
			//Game texture
			unsigned int texture_id;

			//Mouse position relative to Game Window
			Vector2f relative_mouse_pos;

			//Grid management panel
			std::shared_ptr<TileMapPanel> tile_map_panel;

			//Main panel
			std::shared_ptr<MainPanel> main_panel;

			//Game window render event
			void onEvent(std::shared_ptr<Render::ViewportTexture> event);

		public:
			GameWindowPanel() : texture_id{ 0 } {}
			~GameWindowPanel() = default;

			//Panel Name
			std::string getName() const override {
				return "Game Viewport";
			}

			//Static panel name
			static std::string getStaticName() {
				return  "Game Viewport";
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

		//Selected entity event ( Communication between entities panel & components panel )
		struct SelectedEntityEvent : public Events::IEvent {
			Entity::Type selected_entity;
			std::string selected_entity_ref;
			SelectedEntityEvent(Entity::Type selected_entity, std::string const& selected_entity_ref) 
				: selected_entity{ selected_entity }, selected_entity_ref{ selected_entity_ref } {}
		};

		//Entities Management Panel
		class EntitiesPanel : public IPanel {
		private:

			//Set of active entities
			std::set<Entity::Type> entities;

			//BI-Mapping of entity type to string * vice versa
			std::unordered_map<Entity::Type, std::string> entity_to_name;
			std::unordered_map<std::string, Entity::Type> name_to_entity;

			//Selected entity
			Entity::Type selected_entity;

			//Create entity popup
			std::function<void()> createEntityPopUp(std::string const& popup_id);

			//Remove Entity popup
			std::function<void()> removeEntityPopUp(std::string const& popup_id);

			//Clone entity popup
			std::function<void()> cloneEntityPopUp(std::string const& popup_id);

		public:
			EntitiesPanel() : selected_entity{ UINT16_MAX } {}
			~EntitiesPanel() = default;

			//Panel Name
			std::string getName() const override {
				return "Entities Management";
			}

			//Static panel name
			static std::string getStaticName() {
				return "Entities Management";
			}

			//Init
			void init() override;

			//Update
			void update() override;

			//Render
			void render() override;

			//Get entity name
			std::string getEntityName(Entity::Type entity);
		};

		//Components Management Panel
		class ComponentsPanel : public IPanel, public Events::IEventListener<SelectedEntityEvent> {
		private:
			//Current Selected entity
			Entity::Type selected_entity;

			//Current Selected Entity String Reference
			std::string selected_entity_ref;

			//On new selected entity event
			void onEvent(std::shared_ptr<SelectedEntityEvent> event) override;

			//Add Components popup
			std::function<void()> addComponentPopUp(std::string const& popup_id);

			//Set Layer ID popup
			std::function<void()> setLayerIDPopUp(std::string const& popup_id);

			//Component setting error message ( Usage: Editing error popup message )
			std::shared_ptr<std::string> error_msg;

			//Components Map
			std::unordered_map<std::string, Component::Type> comps;

			//Components to UI Function map
			std::unordered_map<std::string, std::function<void(ComponentsPanel&, void*)>> comps_ui;

		public:
			ComponentsPanel() : selected_entity{ UINT16_MAX }, selected_entity_ref{ "" } {}
			~ComponentsPanel() = default;

			//Panel Name
			std::string getName() const override {
				return "Components Management";
			}

			//Static panel name
			static std::string getStaticName() {
				return "Components Management";
			}

			//Init
			void init() override;

			//Update
			void update() override;

			//Render
			void render() override;

			//Set error message for popup
			void setPopUpErrorMsg(std::string const& msg);

			//Add component UI function
			template<typename T>
			void registerCompUIFunc(std::function<void(ComponentsPanel&, T&)> comp_func) {
				if (comps_ui.find(Utility::convertTypeString(typeid(T).name())) != comps_ui.end()) {
					throw std::runtime_error("Component UI function already registered");
				}

				comps_ui.emplace(Utility::convertTypeString(typeid(T).name()), [comp_func](ComponentsPanel& comp_panel, void* comp) { comp_func(comp_panel, *static_cast<T*>(comp)); });
			}
		};

		//Debug Management Panel
		class DebugPanel : public IPanel {
		private:
		public:
			DebugPanel() = default;
			~DebugPanel() = default;

			//Panel Name
			std::string getName() const override {
				return "Debugging Tools";
			}

			//Static panel name
			static std::string getStaticName() {
				return "Debugging Tools";
			}

			//Init
			void init() override;

			//Update
			void update() override;

			//Render
			void render() override;
		};

		//Assets Management Panel
		class ResourcePanel : public IPanel {
		private:
		public:
			ResourcePanel() = default;
			~ResourcePanel() = default;

			//Panel Name
			std::string getName() const override {
				return "Resource Management";
			}

			//Static panel name
			static std::string getStaticName() {
				return "Resource Management";
			}

			//Init
			void init() override;

			//Update
			void update() override;

			//Render
			void render() override;
		};

		//Camera Management Panel
		class CameraPanel : public IPanel {
		private:
			//List of camera entities
			std::unordered_map<Entity::Type, std::string> cam_entities;

			//Entities panel for string reference
			std::shared_ptr<EntitiesPanel> entities_panel;

			//Combo index for selecting camera
			int combo_index;

			//Free camera
			std::shared_ptr<Render::Cam> free_cam;
		public:
			CameraPanel() : combo_index{ 0 } {}
			~CameraPanel() = default;

			//Panel Name
			std::string getName() const override {
				return "Camera Management";
			}

			//Static panel name
			static std::string getStaticName() {
				return "Camera Management";
			}

			//Camera change action
			void cameraChangeAction(Render::Cam& active_cam, Render::Cam& cam_before_change);

			//Get active camera
			Render::Cam getActiveCamera() const;

			//Init
			void init() override;

			//Update
			void update() override;

			//Render
			void render() override;
		};

		//Grid Management Panel
		class TileMapPanel : public IPanel {
		private:
			#ifdef NIKE_BUILD_DLL
			ImVec2 worldToScreen(ImVec2 const& pos, ImVec2 const& render_size);
			#endif // DEBUG

			//Grid scale
			Vector2f grid_scale;

			//Grid thickness
			float grid_thickness;

			//Grid Color
			Vector4f grid_color;

			//Camera panel reference
			std::shared_ptr<CameraPanel> cam_panel;

		public:
			TileMapPanel() : grid_scale(), grid_thickness{ 1.0f }, grid_color{ 1.0f, 1.0f, 1.0f, 1.0f } {}
			~TileMapPanel() = default;

			//Panel Name
			std::string getName() const override {
				return "Tile Map Management";
			}

			//Static panel name
			static std::string getStaticName() {
				return "Tile Map Management";
			}

			//Init
			void init() override;

			//Update
			void update() override;

			//Render
			void render() override;

			//Render grid
			void renderGrid(void* draw_list, Vector2f const& render_size);
		};
	}
}

#endif //!EDITOR_PANELS_HPP
