/*****************************************************************//**
 * \file   mSerialization.cpp
 * \brief  Serialization manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

using json = nlohmann::json;

#ifndef M_SERIALIZATION_HPP
#define M_SERIALIZATION_HPP

namespace NIKE {
	namespace Serialization {

		//Component Registry
		class CompRegister {
		private:
			std::unordered_map<std::string, std::function<json(const void*)>> serializers;
			std::unordered_map<std::string, std::function<void(void*, const json&)>> deserializers;
		public:

			//template<typename T>
			//void registerComponent(std::function<>())
		};


		//Serialization Service
		class NIKE_API Service {
		private:

		public:

		};
	}
}

#endif // !M_SERIALIZATION_HPP
