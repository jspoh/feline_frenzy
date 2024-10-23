/*****************************************************************//**
 * \file   sysImgui.h
 * \brief  imgui system
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef IMGUI_HPP
#define IMGUI_HPP

#include "Managers/ECS/mSystem.h"

namespace NIKESAURUS {
	// All Caps to differentiate from imgui includes
	namespace IMGUI {

		class Manager : public System::ISystem
		{
		public:
			Manager() = default;
			//Update
			void update() override;

			//Default Destructor
			~Manager() override = default;

			//Init
			void init() override;

			// Clean up
			void cleanUp();

			std::string getSysName() override
			{
				return "Imgui System";
			}


		private:
			//Delete Copy Constructor & Copy Assignment
			Manager(Manager const& copy) = delete;
			void operator=(Manager const& copy) = delete;
		};
	}
}


#endif