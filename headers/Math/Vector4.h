/*****************************************************************//**
 * \file   Vector4.h
 * \brief
 *
 * \author jings
 * \date   September 2024
 *********************************************************************/

#pragma once
#pragma warning(push)
#pragma warning(disable: 26495) // Disable the specific warning for this block

class Vector4 {
public:
#pragma warning(disable:4201)
	union {
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
	};
#pragma warning(default:4201)

	Vector4() : x(0), y(0), z(0), w(0) {}
	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

	Vector4 normalize();
	Vector4 normalized() const;

	Vector4 operator+(const Vector4& rhs) const;
	Vector4 operator-(const Vector4& rhs) const;
	Vector4 operator*(float rhs) const;
	Vector4 operator/(float rhs) const;

	Vector4& operator+=(const Vector4& rhs);
	Vector4& operator-=(const Vector4& rhs);
	Vector4& operator*=(float rhs);
	Vector4& operator/=(float rhs);

	float dot(const Vector4& rhs) const;
	float lengthSq() const;
	float length() const;
};

#pragma warning(pop) // Re-enable the warning after this block