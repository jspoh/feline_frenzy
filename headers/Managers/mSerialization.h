/*****************************************************************//**
 * \file   mSerialization.h
 * \brief  Serialization manager
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (100%)
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef M_SERIALIZATION_HPP
#define M_SERIALIZATION_HPP

namespace Serialization {
	class Manager {
	private:
		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

	public:
		//Default Constructor
		Manager() = default;

		// Load Scene from File
		void loadSceneFromFile(const std::string& scene_filepath, std::unordered_map<std::string, Entity::Type>& entities);

	};
}

#endif // !M_SERIALIZATION_HPP
