/*****************************************************************//**
 * \file   Vector2.cpp
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
		Vector2<T> Vector2<T>::operator+(const Vector2<T>& rhs) const {
			return Vector2(x + rhs.x, y + rhs.y);
		}

		template<typename T>
		Vector2<T> Vector2<T>::operator-(const Vector2<T>& rhs) const {
			return Vector2(x - rhs.x, y - rhs.y);
		}

		template<typename T>
		Vector2<T> Vector2<T>::operator*(typename Vector2<T>::type rhs) const {
			return Vector2(x * rhs, y * rhs);
		}

		template<typename T>
		Vector2<T> Vector2<T>::operator/(typename Vector2<T>::type rhs) const {
			return Vector2(x / rhs, y / rhs);
		}

		template<typename T>
		Vector2<T>& Vector2<T>::operator+=(const Vector2<T>& rhs) {
			x += rhs.x;
			y += rhs.y;
			return *this;
		}

		template<typename T>
		Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& rhs) {
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}

		template<typename T>
		Vector2<T>& Vector2<T>::operator*=(typename Vector2<T>::type rhs) {
			x *= rhs;
			y *= rhs;
			return *this;
		}

		template<typename T>
		Vector2<T>& Vector2<T>::operator/=(typename Vector2<T>::type rhs) {
			x /= rhs;
			y /= rhs;
			return *this;
		}

		template<typename T>
		bool Vector2<T>::operator==(const Vector2<T>& rhs) {
			return (this->x == rhs.x) && (this->y == rhs.y);
		}

		template<typename T>
		bool Vector2<T>::operator!=(const Vector2<T>& rhs) {
			return (this->x != rhs.x) || (this->y != rhs.y);
		}

		template<typename T>
		typename Vector2<T>::type Vector2<T>::dot(const Vector2<T>& rhs) const {
			return x * rhs.x + y * rhs.y;
		}

		template<typename T>
		typename Vector2<T>::type Vector2<T>::length() const {
			return sqrt(lengthSq());
		}

		template<typename T>
		typename Vector2<T>::type Vector2<T>::lengthSq() const {
			return x * x + y * y;
		}

		template<typename T>
		Vector2<T> Vector2<T>::normalized() const {
			float len = length();
			return Vector2(x / len, y / len);
		}

		template<typename T>
		Vector2<T> Vector2<T>::normalize() {
			float len = length();
			x /= len;
			y /= len;
			return *this;
		}
	}
}
