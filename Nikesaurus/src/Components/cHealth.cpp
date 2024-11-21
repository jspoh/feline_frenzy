/*****************************************************************//**
 * \file   cHealth.cpp
 * \brief	Health components
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cHealth.h"

namespace NIKE {
	void Health::registerComponents() {
		//Register transform components
		NIKE_ECS_MANAGER->registerComponent<Health>();

		//Register transform for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Health>(
			//Serialize
			[](Health const& comp) -> nlohmann::json {
				return	{
						{ "Lives", comp.lives },
						{ "Health", comp.health },
				};
			},

			//Deserialize
			[](Health& comp, nlohmann::json const& data) {
				comp.lives = data.at("Lives").get<int>();
				comp.health = data.at("Health").get<float>();
			}
		);
	}
}