/*****************************************************************//**
 * \file   ImguiUtils.h
 * \brief  imgui system
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   September 2024
 * All content ? 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef IMGUI_UTILS
#define IMGUI_UTILS

namespace NIKE {

	// This function will be used to update imgui's input if needed
	void imguiInputUpdate();

	// Checks if the file has a valid texture extension (e.g .png, .jpg).
	bool hasValidTextureExtension(const std::filesystem::path& filePath);

	// Checks if the file has a valid audio extension (e.g .wav, .ogg).
	bool hasValidAudioExtension(const std::filesystem::path& filePath);

	// Checks if the file has a valid scene or text extension (e.g .txt, .scn).
	bool hasValidScnTxtExtension(const std::filesystem::path& filePath);

	// Checks if the file has a valid font extension (e.g .ttf).
	bool hasValidFontExtension(const std::filesystem::path& filePath);

	// Checks if the file has a valid vertex shader extension (e.g .vert).
	bool hasValidVertExtension(const std::filesystem::path& filePath);
	// This function will be used to display assets of the given type
	void displayAssetList(const std::string& assetType);

	// Checks if the file has a valid model extension (e.g .txt)
	bool hasValidModelExtension(const std::filesystem::path& filePath);

	// Checks if the file has a valid prefab extension (e.g .prefab)
	bool hasValidPrefabExtension(const std::filesystem::path& filePath);

	// This function will be used to display assets of the given type
	void displayAssetList(const std::string& assetType);

}

#endif
