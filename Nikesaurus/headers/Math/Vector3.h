/*****************************************************************//**
 * \file   Vector3.h
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

namespace NIKESAURUS {
	namespace Math {
		template<typename T>
		class NIKESAURUS_API Vector3 {
		private:
			using type = T;
		public:
			type x, y, z;

			Vector3() : x(0), y(0), z(0) {}
			Vector3(type x, type y, type z) : x(x), y(y), z(z) {}

			Vector3 normalize();
			Vector3 normalized() const;

			Vector3 operator+(const Vector3& rhs) const;
			Vector3 operator-(const Vector3& rhs) const;
			Vector3 operator*(type rhs) const;
			Vector3 operator*(const Matrix_33& mat) const;
			Vector3 operator/(type rhs) const;

			Vector3& operator+=(const Vector3& rhs);
			Vector3& operator-=(const Vector3& rhs);
			Vector3& operator*=(type rhs);
			Vector3& operator/=(type rhs);

			type dot(const Vector3& rhs) const;
			type lengthSq() const;
			type length() const;
		};

		//Predefine templated types for vector 3
		using Vector3b = Vector3<bool>;
		using Vector3s = Vector3<short>;
		using Vector3i = Vector3<int>;
		using Vector3u = Vector3<unsigned int>;
		using Vector3f = Vector3<float>;
		using Vector3d = Vector3<double>;
	}
}
