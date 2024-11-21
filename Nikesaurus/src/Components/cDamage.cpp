/*****************************************************************//**
 * \file   cDamage.cpp
 * \brief	Damage components
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cDamage.h"

namespace NIKE {
	void Damage::registerComponents() {
		//Register transform components
		NIKE_ECS_MANAGER->registerComponent<Damage>();

		//Register transform for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Damage>(
			//Serialize
			[](Damage const& comp) -> nlohmann::json {
				return	{
						{ "Damage", comp.damage },
				};
			},

			//Deserialize
			[](Damage& comp, nlohmann::json const& data) {
				comp.damage = data.at("Damage").get<float>();
			}
		);
	}
}