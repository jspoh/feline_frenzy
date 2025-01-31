/*****************************************************************//**
 * \file   sysRender.h
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (50%)
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (50%)
 * \date   September 2024
 * 
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef RENDER_MANAGER_HPP
#define RENDER_MANAGER_HPP

#include "Managers/ECS/mSystem.h"

#include "Managers/Services/Render/sShader.h"

#include "Components/cRender.h"
#include "Components/cTransform.h"
#include "Core/Engine.h"

namespace NIKE {
	namespace Render {

		//Render Manager
		class Manager : public System::ISystem {
		private:

			//Delete Copy Constructor & Copy Assignment
			Manager(Manager const& copy) = delete;
			void operator=(Manager const& copy) = delete;

			//Helper function to encapsulate rendering entities
			void transformAndRenderEntity(Entity::Type entity, bool debugMode);

			//Helper function to encapsulate rendering text
			void transformAndRenderText(Entity::Type entity);
		public:


			//Constructor
			Manager() = default;

			//Destructor
			~Manager() = default;

			std::string getSysName()
			{
				return "Render System";
			}

			/**
			* update all object's xform
			* */
			void init() override;

			/**
			* update all object's xform
			* */
			void update() override;
		};
	}
}

#endif // !RENDER_MANAGER_HPP
