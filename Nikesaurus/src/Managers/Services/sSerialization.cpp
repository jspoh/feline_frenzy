/*****************************************************************//**
 * \file   mSerialization.cpp
 * \brief  Serialization manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Managers/Services/sSerialization.h"

namespace NIKE {
	/*****************************************************************//**
	* Component Serializer
	*********************************************************************/
	nlohmann::json Serialization::CompSerializer::serializeComponent(unsigned int comp_type, const void* comp) const {
		return serializers.at(comp_type)(comp);
	}

	void Serialization::CompSerializer::deserializeComponent(unsigned int comp_type, void* comp, nlohmann::json const& data) const {
		return deserializers.at(comp_type)(comp, data);
	}

	/*****************************************************************//**
	* Serialization Services
	*********************************************************************/
	nlohmann::json Serialization::Service::serializeEntity(Entity::Type entity) {
		//Get all components from entity
		return nlohmann::json();
	}

	void Serialization::Service::deserializeEntity(Entity::Type entity, nlohmann::json const& ddata) {

	}

	void Serialization::Service::saveEntityToFile(Entity::Type entity, std::string const& file_path) {
		//Json Data
		nlohmann::json data = serializeEntity(entity);

		//Save entity into file
		std::fstream file(file_path, std::ios::out);
		file << data.dump(4);
		file.close();
	}
}

