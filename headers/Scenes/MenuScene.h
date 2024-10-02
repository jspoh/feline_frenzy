/*****************************************************************//**
 * \file   MenuScene.h
 * \brief  
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef MENU_SCENE_HPP
#define MENU_SCENE_HPP

#include "../headers/Systems/sysScene.h"
#include "../headers/Core/Engine.h"

namespace Menu {
	class Scene : public Scenes::IScene {
	private:
		//Create scene specific variables here
		Entity::Type crash_entity;

	public:

		//Dfeault constructor
		Scene() = default;

		//Scene phases
		void load() override;
		void init() override;
		void exit() override;
		void unload() override;
		void loadFromFile(const std::string& scene_filepath, std::unordered_map<std::string, Entity::Type>& entities);

		//Default destructor
		~Scene() override = default;
	};
}

#endif // !MENU_SCENE_HPP
