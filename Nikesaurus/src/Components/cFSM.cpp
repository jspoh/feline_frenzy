/*****************************************************************//**
 * \file   cFSM.cpp
 * \brief  State Machine components
 *
 * \author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu
 * \date   January 2025
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cFSM.h"

namespace NIKE {
	namespace StateMachine{
		void StateMachine::registerComponents()
		{
			// Register components
			NIKE_ECS_MANAGER->registerComponent<StateComponent>();

			// Register for Player serialization
			NIKE_SERIALIZE_SERVICE->registerComponent<StateComponent>(
				// Serialize
				[](StateComponent const& comp) -> nlohmann::json {
					return {
						{ "State" ,  comp.state_id}
					};
				},

				// Deserialize
				[](StateComponent& comp, nlohmann::json const& data) {
					comp.state_id = data.at("State").get<std::string>();

				}
				
			);
		}


	}

}

