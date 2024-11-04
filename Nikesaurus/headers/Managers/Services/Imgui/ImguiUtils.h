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
	// This function is used to check if the file has a valid extension (e.g .txt, .png)
	bool hasValidTextureExtension(const std::filesystem::path& filePath);
	// This function is used to check if the file has a valid extension (e.g .wav, .ogg)
	bool hasValidAudioExtension(const std::filesystem::path& filePath);
	// This function is used to check if the file has a valid extension (e.g .txt, .scn)
	bool hasValidScnTxtExtension(const std::filesystem::path& filePath);
	// This function is used to check if the file has a valid extension (e.g .ttf)
	bool hasValidFontExtension(const std::filesystem::path& filePath);
	// This function is used to check if the file has a valid extension (e.g .vert)
	bool hasValidVertExtension(const std::filesystem::path& filePath);

}

#endif
