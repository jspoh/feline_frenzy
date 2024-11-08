/*****************************************************************//**
 * \file   cGameLogic.cpp
 * \brief	Game logic components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cGameLogic.h"

namespace NIKE {
	void GameLogic::registerComponents() {

		//Register player logic component
		NIKE_ECS_MANAGER->registerComponent<GameLogic::Player>();

		//Register Player For Serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<GameLogic::Player>(
			//Serialize
			[](GameLogic::Player const& comp) -> nlohmann::json {
				return	{
						{ "Script_Path", comp.script }
						};
			},

			//Deserialize
			[](GameLogic::Player& comp, nlohmann::json const& data) {
				comp.script = data.at("Script_Path").get<std::string>();
			}
		);
	}
}