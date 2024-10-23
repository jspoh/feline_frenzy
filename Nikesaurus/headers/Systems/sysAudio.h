/*****************************************************************//**
 * \file   sysAudio.h
 * \brief
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef AUDIO_SYSTEM_HPP
#define AUDIO_SYSTEM_HPP

#include "Managers/ECS/mSystem.h"

namespace NIKESAURUS {
	namespace Audio {
		class Manager : public System::ISystem {
		private:
			//Delete Copy Constructor & Copy Assignment
			Manager(Manager const& copy) = delete;
			void operator=(Manager const& copy) = delete;
		public:
			//Default Constructor
			Manager() = default;

			//Destructor
			~Manager() = default;

			//Animation system
			std::string getSysName() override
			{
				return "Audio System";
			}

			//Override init func
			void init() override;

			//Override update func
			void update() override;
		};
	}
}

#endif //!AUDIO_SYSTEM_HPP
