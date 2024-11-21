///*****************************************************************//**
// * \file   MainMenu.cpp
// * \brief
// * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
// * \date   September 2024
// * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
// *********************************************************************/

#include "Core/pch.h"
#include "Scenes/MainMenu.h"

void Menu::Scene::load() {
	NIKE_ASSETS_SERVICE->reloadAssets("Textures");
	NIKE_ASSETS_SERVICE->reloadAssets("Shaders");
	NIKE_ASSETS_SERVICE->reloadAssets("Audio");
	NIKE_ASSETS_SERVICE->reloadAssets("Models");
	NIKE_ASSETS_SERVICE->reloadAssets("Fonts");
}

void Menu::Scene::init() {
	NIKE_SERIALIZE_SERVICE->loadSceneFromFile("assets/Scenes/main_menu.scn");
}

void Menu::Scene::exit() {

}

void Menu::Scene::unload() {

}
