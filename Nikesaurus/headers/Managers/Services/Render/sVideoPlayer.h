/*****************************************************************//**
 * \file   sVideoPlayer.h
 * \brief
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   March 2025
 *
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef VIDEO_PLAYER_HPP
#define VIDEO_PLAYER_HPP

namespace NIKE {
	namespace VideoPlayer {

		//Video player class
		class Manager {
		private:
		public:

			//Constructor & destructor
			Manager() = default;
			~Manager() = default;

			//Init video
			void init(Render::Video& video);

			//Update
			void update(Render::Video& video);
		};
	}
}

#endif