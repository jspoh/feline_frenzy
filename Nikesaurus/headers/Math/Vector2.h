/*****************************************************************//**
 * \file   Vector2.h
 * \brief  
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

namespace NIKESAURUS {

	class NIKESAURUS_API Vector2 {
	public:
		float x, y;

		Vector2() : x(0), y(0) {}
		Vector2(float x, float y) : x(x), y(y) {}

		Vector2 normalize();
		Vector2 normalized() const;

		Vector2 operator+(const Vector2& rhs) const;
		Vector2 operator-(const Vector2& rhs) const;
		Vector2 operator*(float rhs) const;
		Vector2 operator/(float rhs) const;

		Vector2& operator+=(const Vector2& rhs);
		Vector2& operator-=(const Vector2& rhs);
		Vector2& operator*=(float rhs);
		Vector2& operator/=(float rhs);

		bool operator==(const Vector2& rhs);
		bool operator!=(const Vector2& rhs);

		float dot(const Vector2& rhs) const;
		float lengthSq() const;
		float length() const;
	};
}
