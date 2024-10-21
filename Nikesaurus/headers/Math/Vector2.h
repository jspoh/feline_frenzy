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
	template<typename T>
	class NIKESAURUS_API Vector2 {
	private:
		using type = T;
	public:
		type x, y;

		Vector2() : x(0), y(0) {}
		Vector2(type x, type y) : x(x), y(y) {}

		Vector2 normalize();
		Vector2 normalized() const;

		Vector2 operator+(const Vector2& rhs) const;
		Vector2 operator-(const Vector2& rhs) const;
		Vector2 operator*(type rhs) const;
		Vector2 operator/(type rhs) const;

		Vector2& operator+=(const Vector2& rhs);
		Vector2& operator-=(const Vector2& rhs);
		Vector2& operator*=(type rhs);
		Vector2& operator/=(type rhs);

		bool operator==(const Vector2& rhs);
		bool operator!=(const Vector2& rhs);

		type dot(const Vector2& rhs) const;
		type lengthSq() const;
		type length() const;
	};

	//Predefine templated types for vector 2
	using Vector2b = Vector2<bool>;
	using Vector2s = Vector2<short>;
	using Vector2i = Vector2<int>;
	using Vector2u = Vector2<unsigned int>;
	using Vector2f = Vector2<float>;
	using Vector2d = Vector2<double>;
}
