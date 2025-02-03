/*****************************************************************//**
 * \file   sEditorPanels.h
 * \brief  Level Editor panel implementation
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 *  All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef NDEBUG

#ifndef EDITOR_PANELS_HPP
#define EDITOR_PANELS_HPP

#include "Components/cRender.h"
#include "Components/cTransform.h"
#include "Managers/Services/Assets/sAssets.h"
#include "Managers/ECS/mCoordinator.h"

namespace NIKE {
	namespace LevelEditor {

		//Forward declaration of panels
		class GameWindowPanel;
		class TileMapPanel;
		class ComponentsPanel;
		class PrefabsPanel;

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
			ImVec2 worldToScreen(ImVec2 const& pos, ImVec2 const& render_size, bool use_screen_pos=false);

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

			//Render line to draw list
			void worldLine(ImDrawList* draw_list, Vector2f const& point1, Vector2f const& point2, ImVec2 const& render_size, ImU32 color, float thickness = 1.0f);
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
			MainPanel() :window_flags{ 0 }, b_debug_mode{ false }, b_game_state{ false }, b_grid_state{ false }, b_gizmo_state{ false } {}
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

			//Deserialize config
			void deserializeConfig(nlohmann::json const& config);

			//Init
			void init() override;

			//Render
			void render() override;
		};

		//Entities Management Panel
		class EntitiesPanel :
			public IPanel
		{
		private:

			//Selected entity
			Entity::Type selected_entity;

			//Selected tag
			std::string selected_tag;

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

			//Add tag popup
			std::function<void()> addTagPopUp(std::string const& popup_id);

			//Remove tag popup
			std::function<void()> removeTagPopUp(std::string const& popup_id);

			//Create entity popup
			std::function<void()> createEntityPopUp(std::string const& popup_id);

			//Remove Entity popup
			std::function<void()> removeEntityPopUp(std::string const& popup_id);

			//Clone entity popup
			std::function<void()> cloneEntityPopUp(std::string const& popup_id);

			//Save entity as prefab popup
			std::function<void()> saveEntityAsPrefabPopUp(std::string const& popup_id);

		public:
			EntitiesPanel() : selected_entity{ UINT16_MAX }, selected_tag{}, b_entity_changed { false } {}
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

			//Render
			void render() override;

			//Get selected entity
			Entity::Type getSelectedEntity() const;

			//Unselect entity
			void unselectEntity();

			//Check entity changed
			bool isEntityChanged() const;

			//Check if cusor is in entity
			bool isCursorInEntity(Entity::Type entity) const;
		};

		//Components Management Panel
		class ComponentsPanel : public IPanel {
		private:

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

				//Gizmo Scaling
				float gizmo_scaling;

				//Arrows Offset
				float x_axis_offset;
				float y_axis_offset;

				//boolean for checking interaction with gizmo
				bool b_interacting;

				//Drag up
				bool b_dragging_vert;

				//Drag down
				bool b_dragging_hori;

				//Prev transform
				Transform::Transform prev_transform;

				//Gizmo objects ( transform & color )
				std::unordered_map<std::string, std::pair<Transform::Transform, Vector4i>> objects;

				Gizmo() : mode{ GizmoMode::Translate }, sensitivity{ 1.0f }, gizmo_scaling { 45.f }, x_axis_offset{ 98.5f }, y_axis_offset{ 98.5f }, b_interacting{ false }, b_dragging_vert{ false }, b_dragging_hori{ false }, prev_transform() {}
			};

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

			std::string comp_string_ref;

			////Save Prefab popup
			//std::function<void()> createPrefabPopUp(std::string const& popup_id);

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

			//Add Components popup
			std::function<void()> addComponentPopUp(std::string const& popup_id);

			//Remove Component confirmation popup
			std::function<void()> removeComponentPopUp(std::string const& popup_id);

			//Init
			void init() override;

			//Set comp removal string reference
			void setCompStringRef(std::string const& to_set);

			//Render
			void render() override;

			//Get entity changed boolean
			bool isEntityChanged() const;

			//Set error message for popup
			void setPopUpErrorMsg(std::string const& msg);

			//Set error message for popup
			void setPopUpSuccessMsg(std::string const& msg);

			//Gettor for comps_ui
			std::unordered_map<std::string, std::function<void(ComponentsPanel&, void*)>>& getCompsUI();

			//Get all comps
			std::unordered_map<std::string, Component::Type> getComps() const;

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

		//Prefabs Management panel
		class PrefabsPanel : public IPanel {
		private:

			//Prefab ID
			std::string prefab_id;

			//Prefab copy count
			size_t copy_count;

			//For comp management
			std::string comp_ref;

			// Msg for pop up
			std::shared_ptr<std::string> msg;

			// Reference to component panel
			std::weak_ptr<ComponentsPanel> comps_panel;

			// Reference to entities panel
			std::weak_ptr<EntitiesPanel> entities_panel;

			//Map to array of component type
			std::unordered_map<std::string, std::shared_ptr<void>> prefab_comps;

			//Add component popup
			std::function<void()> addComponentPopUp(std::string const& popup_id);

			//Remove component popup
			std::function<void()> removeComponentPopUp(std::string const& popup_id);

			//Load prefab popup
			std::function<void()> loadPrefabPopUp(std::string const& popup_id);

			//Create prefab popup
			std::function<void()> createPrefabPopUp(std::string const& popup_id);

			//Create entity popup
			std::function<void()> createEntityPopup(std::string const& popup_id);

			//Save prefab
			void savePrefab();

		public:
			PrefabsPanel() : copy_count{ 0 } {}
			~PrefabsPanel() = default;

			//Panel Name
			std::string getName() const override {
				return "Prefab Editor";
			}

			//Static panel name
			static std::string getStaticName() {
				return "Prefab Editor";
			}

			//Init
			void init() override;

			//Render
			void render() override;

			// Accept payload from file management
			void prefabAcceptPayload();

			// For component stuff
			void renderPrefabComponents();
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

			//Render
			void render() override;
		};

		//Audio Management Panel
		class AudioPanel : public IPanel {
		private:

			// Selected channel
			std::string selected_channel_name;

			//Create entity popup
			std::function<void()> createChannelPopUp(std::string const& popup_id);

			//Delete Channel popup
			std::function<void()> deleteChannelPopUp(std::string const& popup_id);

		public:
			AudioPanel() = default;
			~AudioPanel() = default;


			//Panel Name
			std::string getName() const override {
				return "Audio Management";
			}

			//Static panel name
			static std::string getStaticName() {
				return "Audio Management";
			}

			//Init
			void init() override;

			//Render
			void render() override;

		};

		//Resource Management panel
		class ResourcePanel : public IPanel, public Events::IEventListener<Assets::FileDropEvent> {
		private:
			//Directories
			std::vector<std::filesystem::path> directories;

			//Files
			std::vector<std::filesystem::path> files;

			//File Watching Queue
			std::queue<std::function<void()>> file_event_queue;

			//Mutex for thread safety
			std::mutex file_event_mutex;

			//Root Path
			std::string root_path;

			//Current Path
			std::string current_path;

			//Search filter
			std::string search_filter;

			//Icon size
			Vector2f icon_size;

			//Selected file
			std::string selected_asset_id;

			//File payload type string
			std::string payload_typestring;

			//Selected directory mode
			int directory_mode;

			//File dropped
			bool b_file_dropped;

			//Map of file content
			std::unordered_map<std::string, std::string> file_editing_map;

			//Setting error message ( Usage: Editing error popup message )
			std::shared_ptr<std::string> error_msg;

			//Setting success message ( Usage: Editing success popup message )
			std::shared_ptr<std::string> success_msg;

			//Internal asset icon picking
			unsigned int fileIcon(std::filesystem::path const& path);

			//Internal rendering of an asset browser
			void renderAssetsBrowser(std::string const& virtual_path);

#ifdef NIKE_BUILD_DLL
			//File editor state
			struct EditorState {
				int cursor_pos = 0;
			};

			//Text callback
			static int TextCallback(ImGuiInputTextCallbackData* data);
#endif

			//Extract current word being edited
			void extractCurrentWord(std::string const& content, size_t cursor_pos, std::string& buffer);

			//Lua intellisense
			void showLuaIntellisense(std::string& content, size_t cursor_pos, std::string& buffer);

			//Internal rendering of a file editor
			void renderFileEditor();

			//Delete asset popup
			std::function<void()> deleteAssetPopup(std::string const& popup_id);

			//Delete directory content popup
			std::function<void()> deleteDirectoryPopup(std::string const& popup_id);

			//New folder popup
			std::function<void()> newFolderPopup(std::string const& popup_id);

			//Moving file accept payload
			void moveFileAcceptPayload(std::string const& virtual_path);

			//Entities panel for string reference
			std::weak_ptr<EntitiesPanel> entities_panel;

			//On drop file event
			void onEvent(std::shared_ptr<Assets::FileDropEvent> event) override;
		public:
			ResourcePanel() : directory_mode{ 0 }, b_file_dropped{ false } { }
			~ResourcePanel() = default;

			//Panel Name
			std::string getName() const override {
				return "Resource Management";
			}

			//Static panel name
			static std::string getStaticName() {
				return "Resource Management";
			}

			//Thread safe insertion for file event queue
			void pushFileEvent(std::function<void()> callback);

			//Init
			void init() override;

			//Render
			void render() override;
		};

		//Camera Management Panel
		class CameraPanel : public IPanel {
		private:

			//Reference to game window panel
			std::weak_ptr<GameWindowPanel> game_panel;

			//Combo index for selecting camera
			int combo_index;

			int last_dispatched_index;

			//Free camera
			std::shared_ptr<Render::Cam> free_cam;

		public:
			CameraPanel() : combo_index{ 0 }, last_dispatched_index {0} {}
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

			void dispatchCameraChange(Entity::Type cam, const std::string& name);

			//Init
			void init() override;

			//Render
			void render() override;
		};

		//User interface Management Panel
		class UIPanel : public IPanel {
		private:
			//Create button popup
			std::function<void()> createButtonPopup(std::string const& popup_id);

			//Weak reference to entity panel
			std::weak_ptr<EntitiesPanel> entities_panel;
		public:
			UIPanel() = default;
			~UIPanel() = default;

			//Panel Name
			std::string getName() const override {
				return "User Interface Management";
			}

			//Static panel name
			static std::string getStaticName() {
				return "User Interface Management";
			}

			//Init
			void init() override;

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

			// Setting error message ( Usage: Editing error popup message )
			std::shared_ptr<std::string> error_msg;

			// Setting success message ( Usage: Editing success popup message )
			std::shared_ptr<std::string> success_msg;

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

			//Render
			void render() override;

			void saveGrid(std::filesystem::path);

			void removeGrid(std::filesystem::path);

			//Save grid popup
			std::function<void()> saveGridPopUp(std::string const& popup_id);

			//Render grid
			void renderGrid(void* draw_list, Vector2f const& render_size);

			//Get grid editing mode
			bool checkGridEditing() const;

			//Get grid snapping
			bool checkGridSnapping() const;
		};

		//Scenes Management panel
		class ScenesPanel : public IPanel {
		private:

			//Error msg
			std::shared_ptr<std::string> err_msg;

			//Success msg
			std::shared_ptr<std::string> success_msg;

			// For storing editing layer mask id
			unsigned int edit_mask_id;

			bool bit_state;

			// For storing selected layer index
			unsigned int selected_layer_index;

			// For storing bit_position
			unsigned int bit_position;

			// To store layer names
			std::vector<std::string> layer_names;

			// Ref to tile map
			std::weak_ptr<TileMapPanel> tile_panel;

			//Create scene popup
			std::function<void()> createScenePopup(std::string const& popup_id);

			//Delete scene popup
			std::function<void()> deleteScenePopup(std::string const& popup_id);

			//Edit Mask popup
			std::function<void()> editBitMaskPopup(std::string const& popup_id);

		public:
			ScenesPanel() : edit_mask_id{ 0 }, selected_layer_index{ 0 }, bit_position{ 0 }, bit_state{ false } {}
			~ScenesPanel() = default;

			//Panel Name
			std::string getName() const override {
				return "Scenes Management";
			}

			//Static panel name
			static std::string getStaticName() {
				return "Scenes Management";
			}

			//Update layer names
			void updateLayerNames();

			//Set error message for popup
			void setPopUpErrorMsg(std::string const& msg);

			//Init
			void init() override;

			//Render
			void render() override;
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

			//Render accept payload
			void renderAcceptPayload();

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

			//Render
			void render() override;
		};
	}
}

#endif //!EDITOR_PANELS_HPP


#endif