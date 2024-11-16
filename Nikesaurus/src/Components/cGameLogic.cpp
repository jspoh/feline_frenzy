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

	nlohmann::json ScriptSerialize(GameLogic::Script const& comp) {
		return	{
				{ "Script_Path", comp.script_path },
				{ "Function", comp.function },
		};
	}

	void ScriptDeserialize(GameLogic::Script& comp, nlohmann::json const& data) {
		comp.script_path = data.at("Script_Path").get<std::string>();
		comp.function = data.at("Function").get<std::string>();
	}

	void GameLogic::registerComponents() {

		//Register logic components
		NIKE_ECS_MANAGER->registerComponent<GameLogic::Movement>();
		NIKE_ECS_MANAGER->registerComponent<GameLogic::StateMachine>();

		//Register Player For Serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<GameLogic::Movement>(
			//Serialize
			[](GameLogic::Movement const& comp) -> nlohmann::json {
				return	ScriptSerialize(comp.script);
			},

			//Deserialize
			[](GameLogic::Movement& comp, nlohmann::json const& data) {
				ScriptDeserialize(comp.script, data);
			}
		);
	}
}