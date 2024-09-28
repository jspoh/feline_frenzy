#pragma once

#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include "../headers/Systems/sysAudio.h"

namespace Asset
{

	class Manager
	{
	public:

		Manager() = default;
		~Manager() = default;

		// Load and get texture
		// void LoadTexture(std::string textName, std::string path);
		// sf::Texture& GetTexture(std::string textName);

		// Load and get font
		// void LoadFont(std::string textName, std::string path);
		// sf::Font& GetFont(std::string textName);

		// Load sound and music using the respective functions
		void LoadSound(std::string file_path, std::string audio_tag);
		void LoadMusic(std::string file_path, std::string audio_tag);
		void CreateAudioGroup(std::string const& audio_group_tag);

		// Gettors
		Audio::NE_AUDIO GetAudio(std::string audio_tag);
		Audio::NE_AUDIO_GROUP GetAudioGroup(std::string const& tag);

		// Checks
		bool IsValidGroup(std::string const& audio_group_tag);
		bool IsValidAudio(std::string const& audio_tag);

	private:
		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;
		// Containers to store textures and fonts
		// std::unordered_map<std::string, sf::Texture> textureList;
		// std::unordered_map<std::string, sf::Font> fontList;
		
		// Audio object will be value
		// Tag of filepath audio will be key
		Audio::AUDIO_GROUP audio_group_list;
		Audio::AUDIO_MAP audio_list;
	};
}

#endif