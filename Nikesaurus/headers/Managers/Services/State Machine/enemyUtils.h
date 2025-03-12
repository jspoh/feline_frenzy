/*****************************************************************//**
 * \file   enemyUtils.h
 * \brief  Enemy system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (70%)
 * \co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (30%)
 * \date   November 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Managers/ECS/mSystem.h"
#include "Components/cPhysics.h"
#include "Components/cEnemy.h"
#include "Managers/Services/Lua/sLua.h"

namespace NIKE {
	namespace Enemy {
		// Enemey logic functions

		// Shoot bullet
		void shootBullet(const Entity::Type& enemy, const Entity::Type& player);

		// Boss shoot
		void bossShoot(const Entity::Type& enemy, const Entity::Type& player);

		// Standard enemy moving function
		void moveAlongPath(Entity::Type entity, int x_index, int y_index, float speed, float cell_offset);

		bool isWithinGridRange(const Entity::Type& enemy, const Entity::Type& player);

		// Standard enemy moving function
		//void chasing(Pathfinding::Path& path, Transform::Transform& enemy, Transform::Transform& player);

		//bool hasTargetMoved(const Vector2f& target_pos, const Pathfinding::Path& path) const;
	}
}
#endif //!ENEMY_HPP