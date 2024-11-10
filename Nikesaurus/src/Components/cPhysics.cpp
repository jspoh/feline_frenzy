/*****************************************************************//**
 * \file   cPhysics.cpp
 * \brief	Physics components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cPhysics.h"

namespace NIKE {
	void Physics::registerComponents() {
		//Register physics components
		NIKE_ECS_MANAGER->registerComponent<Physics::Dynamics>();
		NIKE_ECS_MANAGER->registerComponent<Physics::Collider>();

		//Register Dynamics for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Physics::Dynamics>(
			//Serialize
			[](Physics::Dynamics const& comp) -> nlohmann::json {
				return	{
						{ "Max_Speed", comp.max_speed },
						{ "Drag", comp.drag },
						{ "Mass", comp.mass },
						{ "Velocity", comp.velocity.toJson() },
						{ "Force", comp.force.toJson() }
						};
			},

			//Deserialize
			[](Physics::Dynamics& comp, nlohmann::json const& data) {
				comp.max_speed = data.at("Max_Speed").get<float>();
				comp.drag = data.at("Drag").get<float>();
				comp.mass = data.at("Mass").get<float>();
				comp.velocity.fromJson(data.at("Velocity"));
				comp.force.fromJson(data.at("Force"));
			}
		);

		//Register Collider for serializarion
		NIKE_SERIALIZE_SERVICE->registerComponent<Physics::Collider>(
			//Serialize
			[](Physics::Collider const& comp) -> nlohmann::json {
				return	{
						{ "B_Collided", comp.b_collided },
						{ "Resolution", static_cast<int>(comp.resolution) }
						};
			},

			//Deserialize
			[](Physics::Collider& comp, nlohmann::json const& data) {
				comp.b_collided = data.at("B_Collided").get<bool>();
				comp.resolution = static_cast<Resolution>(data.at("Resolution").get<int>());
			}
		);
	}
}