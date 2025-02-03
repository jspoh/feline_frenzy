/*****************************************************************//**
 * \file   mSerialization.h
 * \brief  Serialization manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#include "Core/Engine.h"
#include "Managers/ECS/mCoordinator.h"

#ifndef M_SERIALIZATION_HPP
#define M_SERIALIZATION_HPP

namespace NIKE {
	namespace Serialization {

		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		//Component Serializer
		class CompSerializer {
		private:
			//Comp serializers & deserializers
			std::unordered_map<std::string, std::function<nlohmann::json(const void*)>> serializers;
			std::unordered_map<std::string, std::function<void(void*, nlohmann::json const&)>> deserializers;

			//Comp override serializers & deserializers
			std::unordered_map<std::string, std::function<nlohmann::json(const void*, const void*)>> override_serializers;
			std::unordered_map<std::string, std::function<void(void*, nlohmann::json const&)>> override_deserializers;

		public:

			//Register component for serialization
			template<typename T>
			void registerComponent(std::function<nlohmann::json(T const&)> serialize, std::function<void(T&, nlohmann::json const&)> deserialize, std::function<nlohmann::json(T const&, T const&)> override_serialize, std::function<void(T&, nlohmann::json const&)> override_deserialize) {
				//Emplace serializer functions
				serializers.emplace(std::piecewise_construct,
					std::forward_as_tuple(Utility::convertTypeString(typeid(T).name())),
					std::forward_as_tuple([serialize](const void* comp) -> nlohmann::json { return serialize(*static_cast<const T*>(comp)); }));

				//Emplace deserializers functions
				deserializers.emplace(std::piecewise_construct,
					std::forward_as_tuple(Utility::convertTypeString(typeid(T).name())),
					std::forward_as_tuple([deserialize](void* comp, nlohmann::json const& data) { deserialize(*static_cast<T*>(comp), data); }));

				//Emplace serializer functions
				override_serializers.emplace(std::piecewise_construct,
					std::forward_as_tuple(Utility::convertTypeString(typeid(T).name())),
					std::forward_as_tuple([override_serialize](const void* comp, const void* other_comp) -> nlohmann::json { return override_serialize(*static_cast<const T*>(comp), *static_cast<const T*>(other_comp)); }));

				//Emplace deserializers functions
				override_deserializers.emplace(std::piecewise_construct,
					std::forward_as_tuple(Utility::convertTypeString(typeid(T).name())),
					std::forward_as_tuple([override_deserialize](void* comp, nlohmann::json const& data) { override_deserialize(*static_cast<T*>(comp), data); }));
			}

			//Serialize component
			nlohmann::json serializeComponent(std::string const& comp_name, const void* comp) const;

			//Deserialize component
			void deserializeComponent(std::string const& comp_name, void* comp, nlohmann::json const& data) const;

			//Serialize override component
			nlohmann::json serializeOverrideComponent(std::string const& comp_name, const void* comp, const void* other_comp) const;

			//Deserialize Override component
			void deserializeOverrideComponent(std::string const& comp_name, void* comp, nlohmann::json const& data) const;
		};

		//Serialization Service
		class NIKE_API Service {
		private:
			//Component Serialization Registry
			std::unique_ptr<CompSerializer> comp_registry;

			//Comp adding functions
			std::unordered_map<std::string, std::function<std::shared_ptr<void>()>> comp_funcs;

			//Serialize Entity
			nlohmann::json serializeEntity(Entity::Type entity);

			//Deserialize Entity
			void deserializeEntity(Entity::Type entity, nlohmann::json const& data);

			//current scene path string when (de-serialising)
			std::string curr_scene_file = "";

		public:
			Service() : comp_registry{ std::make_unique<CompSerializer>() } {}
			~Service() = default;

			//Register component for serialization
			template<typename T>
			void registerComponent(std::function<nlohmann::json(T const&)> serialize, std::function<void(T&, nlohmann::json const&)> deserialize, std::function<nlohmann::json(T const&, T const&)> override_serialize, std::function<void(T&, nlohmann::json const&)> override_deserialize) {
				comp_registry->registerComponent<T>(serialize, deserialize, override_serialize, override_deserialize);
			}

			//Register component adding
			template<typename T>
			void registerComponentAdding() {

				//Create a default component of type T adding function
				comp_funcs[Utility::convertTypeString(typeid(T).name())] = []() -> std::shared_ptr<void> {
					return std::make_shared<T>();
					};
			}

			//Get component adding functions
			std::unordered_map<std::string, std::function<std::shared_ptr<void>()>>const& getCompFuncs() const;

			// Save grid to .map file
			void saveGridToFile(const std::string& file_path);

			void loadGridFromFile(const std::string& file_path);

			//Serialize Prefab
			void savePrefab(std::unordered_map<std::string, std::shared_ptr<void>> const& comps, std::string const& file_path);

			//Deserialize Prefab
			void loadPrefab(std::unordered_map<std::string, std::shared_ptr<void>>& comps, std::string const& file_path);

			//Serialize prefab overrides
			nlohmann::json serializePrefabOverrides(Entity::Type entity, std::string const& prefab_id);

			//Deserialize prefab overrides
			void deserializePrefabOverrides(Entity::Type entity, nlohmann::json const& data);

			//Serialize Entity into file path
			void saveEntityToFile(Entity::Type entity, std::string const& file_path);

			//Load entity from file path
			void loadEntityFromFile(Entity::Type entity, std::string const& file_path);

			//Load entity from prefab
			void loadEntityFromPrefab(Entity::Type entity, std::string const& prefab_id);

			//Get current scene file path
			std::string const& getCurrSceneFile() const;

			//Serialize Scene into file path
			void saveSceneToFile(std::string const& file_path);

			//Load scene from file path
			void loadSceneFromFile(std::string const& file_path);

			//Load json file path
			nlohmann::json loadJsonFile(std::string const& file_path);
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif // !M_SERIALIZATION_HPP
