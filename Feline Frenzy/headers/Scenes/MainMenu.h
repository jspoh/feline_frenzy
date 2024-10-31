///*****************************************************************//**
// * \file   MainMenu.h
// * \brief
// * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
// * \date   September 2024
// * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
// *********************************************************************/
//
#pragma once

#ifndef MENU_SCENE_HPP
#define MENU_SCENE_HPP

namespace Menu {
	class Scene : public NIKE::Scenes::IScene {
	private:

	public:

		//Dfeault constructor
		Scene() = default;

		//Scene phases
		void load() override;
		void init() override;
		void exit() override;
		void unload() override;

		//Default destructor
		~Scene() override = default;
	};
}

#endif // !MENU_SCENE_HPP