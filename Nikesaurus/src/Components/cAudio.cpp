/*****************************************************************//**
 * \file   cAudio.cpp
 * \brief	Animation components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cAudio.h"

namespace NIKE {
	void Audio::registerComponents() {
		//Register audio components
		NIKE_ECS_MANAGER->registerComponent<Audio::SFX>();

		//Register SFX for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Audio::SFX>(
			//Serialize
			[](Audio::SFX const& comp) -> nlohmann::json {
				return	{
						{ "B_Play_SFX", comp.b_play_sfx },
						{ "Audio_ID", comp.audio_id },
						{ "Channel_Group_ID", comp.channel_group_id },
						{ "Volume", comp.volume },
						{ "Pitch", comp.pitch }
						};
			},

			//Deserialize
			[](Audio::SFX& comp, nlohmann::json const& data) {
				comp.b_play_sfx = data.at("B_Play_SFX").get<bool>();
				comp.audio_id = data.at("Audio_ID").get<std::string>();
				comp.channel_group_id = data.at("Channel_Group_ID").get<std::string>();
				comp.volume = data.at("Volume").get<float>();
				comp.pitch = data.at("Pitch").get<float>();
			}
		);

		//Register SFX for level editor UI

	}
}