/*****************************************************************//**
 * \file   Vector4.h
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once
#pragma warning(push)
#pragma warning(disable: 26495) // Disable the specific warning for this block

namespace NIKE {
	namespace Math {
		template<typename T>
		class Vector4 {
		private:
			using type = T;
		public:
#pragma warning(disable:4201)
			union {
				struct { type x, y, z, w; };
				struct { type r, g, b, a; };
			};
#pragma warning(default:4201)

			Vector4() : x(0), y(0), z(0), w(0) {}
			Vector4(type x, type y, type z, type w) : x(x), y(y), z(z), w(w) {}

			Vector4 normalize();
			Vector4 normalized() const;

			Vector4 operator+(const Vector4& rhs) const;
			Vector4 operator-(const Vector4& rhs) const;
			Vector4 operator*(type rhs) const;
			Vector4 operator/(type rhs) const;

			Vector4& operator+=(const Vector4& rhs);
			Vector4& operator-=(const Vector4& rhs);
			Vector4& operator*=(type rhs);
			Vector4& operator/=(type rhs);

			type dot(const Vector4& rhs) const;
			type lengthSq() const;
			type length() const;

			//Serialize to json
			nlohmann::json toJson() const;

			//Deserialize from json
			void fromJson(nlohmann::json const& data);
		};

		#include "Vector4.inl"

		//Predefine templated types for vector 3
		using Vector4b = Vector4<bool>;
		using Vector4s = Vector4<short>;
		using Vector4i = Vector4<int>;
		using Vector4u = Vector4<unsigned int>;
		using Vector4f = Vector4<float>;
		using Vector4d = Vector4<double>;

#pragma warning(pop) // Re-enable the warning after this block
	}
}
