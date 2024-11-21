/*****************************************************************//**
 * \file   cShooting.cpp
 * \brief  Shooting components
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cShooting.h"

namespace NIKE {
	void Shooting::registerComponents() {
		//Register transform components
		NIKE_ECS_MANAGER->registerComponent<Shooting>();

		//Register transform for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Shooting>(
			//Serialize
			[](Shooting const& comp) -> nlohmann::json {
				return	{
						{ "Cooldown", comp.cooldown },
						{ "LastShotTime", comp.last_shot_time },
				};
			},

			//Deserialize
			[](Shooting& comp, nlohmann::json const& data) {
				comp.cooldown = data.at("Cooldown").get<float>();
				comp.last_shot_time = data.at("LastShotTime").get<float>();
			}
		);
	}
}