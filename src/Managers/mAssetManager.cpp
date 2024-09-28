
#include "../headers/Core/stdafx.h"
#include "../headers/Managers/mAssetManager.h"
#include "../headers/Core/Engine.h"

namespace Asset
{
	void Manager::LoadSound(std::string file_path, std::string audio_tag)
	{
		Audio::NE_AUDIO to_add = NIKEEngine.accessSystem<Audio::Manager>()->NEAudioLoadSound(file_path);
		audio_list[audio_tag] = to_add;
	}

	void Manager::LoadMusic(std::string file_path, std::string audio_tag)
	{
		Audio::NE_AUDIO to_add = NIKEEngine.accessSystem<Audio::Manager>()->NEAudioLoadMusic(file_path);
		audio_list[audio_tag] = to_add;
	}

	Audio::NE_AUDIO Manager::GetAudio(std::string audio_tag)
	{
		return audio_list[audio_tag];
	}

	void Manager::CreateAudioGroup(std::string const& audio_group_tag)
	{	
		Audio::NE_AUDIO_GROUP to_add = NIKEEngine.accessSystem<Audio::Manager>()->CreateAudioGroup(audio_group_tag);

		// Check if the group already exists in the map
		if (audio_group_list.find(audio_group_tag) != audio_group_list.end())
		{
			throw std::runtime_error("AUDIO GROUP ALREADY EXISTS"); 
		}

		// Push into audio group map
		audio_group_list[audio_group_tag] = to_add;
	}

	Audio::NE_AUDIO_GROUP Manager::GetAudioGroup(std::string const& tag)
	{
		return audio_group_list[tag];
	}

	bool Manager::IsValidGroup(std::string const& audio_group_tag)
	{
		// Check if the group already exists in the map
		if (audio_group_list.find(audio_group_tag) != audio_group_list.end())
		{
			return true;
		}
		return false;
	}

	bool Manager::IsValidAudio(std::string const& audio_tag)
	{
		// Check if the group already exists in the map
		if (audio_list.find(audio_tag) != audio_list.end())
		{
			return true;
		}
		return false;
	}
}


