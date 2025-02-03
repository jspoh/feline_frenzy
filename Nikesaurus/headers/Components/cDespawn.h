/*****************************************************************//**
 * \file   cDespawn.h
 * \brief  Defines the components used for Despawning entity
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *********************************************************************/

#pragma once

#ifndef C_DESPAWN_HPP
#define C_DESPAWN_HPP

namespace NIKE {
	namespace Despawn {
		struct Lifetime {
			float current_lifetime;		// Duration that entity has existed for
			float max_lifetime;			// Despawn once entity exists for this duration

			Lifetime() : current_lifetime{ .0f }, max_lifetime{ 10.f } {};
			Lifetime(float const& max_lifetime) : current_lifetime{ .0f }, max_lifetime{ max_lifetime } {};
		};

		void registerComponents();

		void registerEditorComponents();
	}
}


#endif // !C_DESPAWN_HPP