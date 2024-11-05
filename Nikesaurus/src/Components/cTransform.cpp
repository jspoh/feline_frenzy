/*****************************************************************//**
 * \file   cTransform.cpp
 * \brief	Animation components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cTransform.h"

namespace NIKE {
	void Transform::registerComponents() {
		//Register transform components
		NIKE_ECS_MANAGER->registerComponent<Transform>();

		//Register transform for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Transform>(
			//Serialize
			[](Transform const& comp) -> nlohmann::json {
				return	{
						{ "Position", comp.position.toJson() },
						{ "Scale", comp.scale.toJson() },
						{ "Rotation", comp.rotation }
						};
			},

			//Deserialize
			[](Transform& comp, nlohmann::json const& data) {
				comp.position.fromJson(data.at("Position"));
				comp.scale.fromJson(data.at("Scale"));
				comp.rotation = data.at("Rotation").get<float>();
			}
		);
	}
}