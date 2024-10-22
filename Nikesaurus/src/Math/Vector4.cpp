/*****************************************************************//**
 * \file   Vector4<T>.cpp
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"

namespace NIKESAURUS {
	namespace Math {
		template<typename T>
		Vector4<T> Vector4<T>::operator+(const Vector4<T>& rhs) const {
			return Vector4<T>(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
		}

		template<typename T>
		Vector4<T> Vector4<T>::operator-(const Vector4<T>& rhs) const {
			return Vector4<T>(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
		}

		template<typename T>
		Vector4<T> Vector4<T>::operator*(typename Vector4<T>::type rhs) const {
			return Vector4<T>(x * rhs, y * rhs, z * rhs, w * rhs);
		}

		template<typename T>
		Vector4<T> Vector4<T>::operator/(typename Vector4<T>::type rhs) const {
			return Vector4<T>(x / rhs, y / rhs, z / rhs, w / rhs);
		}

		template<typename T>
		Vector4<T>& Vector4<T>::operator+=(const Vector4<T>& rhs) {
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			w += rhs.w;
			return *this;
		}

		template<typename T>
		Vector4<T>& Vector4<T>::operator-=(const Vector4<T>& rhs) {
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			w -= rhs.w;
			return *this;
		}

		template<typename T>
		Vector4<T>& Vector4<T>::operator*=(typename Vector4<T>::type rhs) {
			x *= rhs;
			y *= rhs;
			z *= rhs;
			w *= rhs;
			return *this;
		}

		template<typename T>
		Vector4<T>& Vector4<T>::operator/=(typename Vector4<T>::type rhs) {
			x /= rhs;
			y /= rhs;
			z /= rhs;
			w /= rhs;
			return *this;
		}

		template<typename T>
		typename Vector4<T>::type Vector4<T>::dot(const Vector4<T>& rhs) const {
			return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
		}

		template<typename T>
		typename Vector4<T>::type Vector4<T>::length() const {
			return sqrt(lengthSq());
		}

		template<typename T>
		typename Vector4<T>::type Vector4<T>::lengthSq() const {
			return x * x + y * y + z * z + w * w;
		}

		template<typename T>
		Vector4<T> Vector4<T>::normalized() const {
			typename Vector4<T>::type len = length();
			return Vector4<T>(x / len, y / len, z / len, w / len);
		}

		template<typename T>
		Vector4<T> Vector4<T>::normalize() {
			typename Vector4<T>::type len = length();
			x /= len;
			y /= len;
			z /= len;
			w /= len;
			return *this;
		}
	}
}
