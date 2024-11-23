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
#include "Components/cTransform.h"

namespace NIKE {
	namespace LevelEditor {

		//Forward declaration of panels
		class GameWindowPanel;
		class TileMapPanel;
		class ComponentsPanel;

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

			#ifdef NIKE_BUILD_DLL
			//World to screen
			ImVec2 worldToScreen(ImVec2 const& pos, ImVec2 const& render_size);

			//Render filled rectangle to draw list
			void worldRectFilled(ImDrawList* draw_list, Transform::Transform const& e_transform, ImVec2 const& render_size, ImU32 color, float rounding = 0.0f);

			//Render rectangle to draw list
			void worldRect(ImDrawList* draw_list, Transform::Transform const& e_transform, ImVec2 const& render_size, ImU32 color, float rounding = 0.0f, float thickness = 1.0f);

			//Render filled quad to draw list ( Basically rectangle but with rotated )
			void worldQuadFilled(ImDrawList* draw_list, Transform::Transform const& e_transform, ImVec2 const& render_size, ImU32 color);

			//Render quad to draw list ( Basically rectangle but with rotated )
			void worldQuad(ImDrawList* draw_list, Transform::Transform const& e_transform, ImVec2 const& render_size, ImU32 color, float thickness = 1.0f);

			//Render filled circle to draw list
			void worldCircleFilled(ImDrawList* draw_list, Transform::Transform const& e_transform, ImVec2 const& render_size, ImU32 color);

			//Render circle to draw list
			void worldCircle(ImDrawList* draw_list, Transform::Transform const& e_transform, ImVec2 const& render_size, ImU32 color, float thickness = 1.0f);

			//Render filled triangle to draw list
			void worldTriangleFilled(ImDrawList* draw_list, Transform::Transform const& e_transform, ImGuiDir dir, ImVec2 const& render_size, ImU32 color);
			#endif // Only in nike build
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

			//Boolean for enabling/disabling transform gizmo
			bool b_gizmo_state;

			//Set Game State
			void setGameState(bool state);

		public:
			MainPanel() :window_flags{ 0 }, b_debug_mode{ false }, b_game_state{ true }, b_grid_state{ false }, b_gizmo_state{ false } {}
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

			//Public get gizmo state
			bool getGizmoState() const;

			//Init
			void init() override;

			//Update
			void update() override;

			//Render
			void render() override;
		};

		//Entities management structure
		struct EditorEntity {
			bool b_locked;

			EditorEntity() :b_locked{ false } {}
			EditorEntity(bool b_locked) : b_locked{ b_locked }{}
		};

		//Entities Management Panel
		class EntitiesPanel : public IPanel {
		private:
			//Sort entities
			struct EntitySorter{
				bool operator()(Entity::Type const& e1, Entity::Type const& e2) const {
					return e1 < e2;
				}
			};

			//Set of active entities
			std::map<Entity::Type, EditorEntity, EntitySorter> entities;

			//BI-Mapping of entity type to string * vice versa
			std::unordered_map<Entity::Type, std::string> entity_to_name;
			std::unordered_map<std::string, Entity::Type> name_to_entity;

			//Selected entity
			Entity::Type selected_entity;

			//Entity changed event boolean
			bool b_entity_changed;

			//Reference to game window panel
			std::weak_ptr<GameWindowPanel> game_panel;

			//Reference to game window panel
			std::weak_ptr<TileMapPanel> tilemap_panel;

			//Reference to component panel
			std::weak_ptr<ComponentsPanel> comp_panel;

			//Error msg
			std::shared_ptr<std::string> error_msg;

			//Create entity popup
			std::function<void()> createEntityPopUp(std::string const& popup_id);

			//Remove Entity popup
			std::function<void()> removeEntityPopUp(std::string const& popup_id);

			//Clone entity popup
			std::function<void()> cloneEntityPopUp(std::string const& popup_id);
		public:
			EntitiesPanel() : selected_entity{ UINT16_MAX }, b_entity_changed{ false } {}
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

			//Get selected entity
			Entity::Type getSelectedEntity() const;
			
			//Get selected entity name
			std::optional<std::string> getSelectedEntityName() const;

			//Unselect entity
			void unselectEntity();

			//Get selected entity editor variables
			std::optional<std::reference_wrapper<LevelEditor::EditorEntity>> getSelectedEntityEditor();

			//Lock all entities
			void lockAllEntities();

			//Unlock all entities
			void unlockAllEntities();

			//Check entity changed
			bool isEntityChanged() const;

			//Check if cusor is in entity
			bool isCursorInEntity(Entity::Type entity) const;
		};

		//Gizmo modes
		enum class GizmoMode {
			Translate = 0,
			Scale,
			Rotate
		};

		//Gizmo object
		struct Gizmo {
			//Gizmo mode
			GizmoMode mode;

			//Gizmo sensitivity
			float sensitivity;

			//boolean for checking interaction with gizmo
			bool b_interacting;

			//Drag up
			bool b_dragging_vert;

			//Drag down
			bool b_dragging_hori;

			//Prev transform
			Transform::Transform prev_transform;

			//Gizmo objects
			std::unordered_map<std::string, Transform::Transform> objects;

			Gizmo() : mode{ GizmoMode::Translate }, sensitivity{ 1.0f }, b_interacting{ false }, b_dragging_vert{ false }, b_dragging_hori{ false }, prev_transform() {}
		};

		//Components Management Panel
		class ComponentsPanel : public IPanel {
		private:

			//Reference to game window panel
			std::weak_ptr<EntitiesPanel> entities_panel;

			//Reference to game window panel
			std::weak_ptr<GameWindowPanel> game_panel;

			//Reference to main panel
			std::weak_ptr<MainPanel> main_panel;

			//Reference to tilemap panel
			std::weak_ptr<TileMapPanel> tilemap_panel;

			//Boolean to signal dragging of entity
			bool b_dragging_entity;

			//Boolean to manage changing gizmos
			Gizmo gizmo;

			//Gizmo interaction
			void interactGizmo();

			//Add Components popup
			std::function<void()> addComponentPopUp(std::string const& popup_id);

			//Save Prefab popup
			std::function<void()> savePrefabPopUp(std::string const& popup_id);

			//Set Layer ID popup
			std::function<void()> setLayerIDPopUp(std::string const& popup_id);

			//Component setting error message ( Usage: Editing error popup message )
			std::shared_ptr<std::string> error_msg;

			//Component setting success message ( Usage: Editing success popup message )
			std::shared_ptr<std::string> success_msg;

			//Components Map
			std::unordered_map<std::string, Component::Type> comps;

			//Components to UI Function map
			std::unordered_map<std::string, std::function<void(ComponentsPanel&, void*)>> comps_ui;

		public:
			ComponentsPanel() : b_dragging_entity{ false }, gizmo() {};
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

			//Get entity changed boolean
			bool isEntityChanged() const;

			//Set error message for popup
			void setPopUpErrorMsg(std::string const& msg);

			//Set error message for popup
			void setPopUpSuccessMsg(std::string const& msg);

			//Add component UI function
			template<typename T>
			void registerCompUIFunc(std::function<void(ComponentsPanel&, T&)> comp_func) {
				if (comps_ui.find(Utility::convertTypeString(typeid(T).name())) != comps_ui.end()) {
					throw std::runtime_error("Component UI function already registered");
				}

				comps_ui.emplace(Utility::convertTypeString(typeid(T).name()), [comp_func](ComponentsPanel& comp_panel, void* comp) { comp_func(comp_panel, *static_cast<T*>(comp)); });
			}

			//Render selected entity bounding box
			void renderEntityBoundingBox(void* draw_list, Vector2f const& render_size);

			//Render selected entity gizmo
			void renderEntityGizmo(void* draw_list, Vector2f const& render_size);

			//Check if entity is being dragged
			bool checkEntityDragged() const;

			//Check if there is interaction with gizmo
			bool checkGizmoInteraction() const;

			//Check if entity is snapping to grid
			bool checkGridSnapping() const;
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
			std::weak_ptr<EntitiesPanel> entities_panel;

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
			//Grid thickness
			float grid_thickness;

			//Grid Color
			Vector4f grid_color;

			//Boolean for grid mode
			bool b_grid_edit;

			//Booelean for snapping entities to grid
			bool b_snap_to_grid;

			//Reference to game window panel
			std::weak_ptr<EntitiesPanel> entities_panel;

		public:
			TileMapPanel() : grid_thickness{ 1.0f }, grid_color{ 1.0f, 1.0f, 1.0f, 1.0f }, b_grid_edit{ false }, b_snap_to_grid{ false } {}
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

			//Get grid editing mode
			bool checkGridEditing() const;

			//Get grid snapping
			bool checkGridSnapping() const;
		};

		//Game Window Panel
		class GameWindowPanel : public IPanel, public Events::IEventListener<Render::ViewportTexture> {
		private:
			//Game texture
			unsigned int texture_id;

			//Mouse position relative to Game Window
			Vector2f window_mouse_pos;

			//Mouse position relative to world
			Vector2f world_mouse_pos;

			//Grid management panel
			std::weak_ptr<TileMapPanel> tile_map_panel;

			//Main panel reference
			std::weak_ptr<MainPanel> main_panel;

			//Entities panel reference
			std::weak_ptr<ComponentsPanel> comps_panel;

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

			//Get world mouse position ( Center of world being 0,0 )
			Vector2f getWorldMousePos() const;

			//Get window mouse position ( Top left of game window 0,0 )
			Vector2f getWindowMousePos() const;

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
