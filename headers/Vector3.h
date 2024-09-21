/*****************************************************************//**
 * \file   Vector3.h
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#pragma once

class Vector3 {
public:
	float x, y, z;

	Vector3() : x(0), y(0), z(0) {}
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

	Vector3 normalize();
	Vector3 normalized() const;

	Vector3 operator+(const Vector3& rhs) const;
	Vector3 operator-(const Vector3& rhs) const;
	Vector3 operator*(float rhs) const;
	Vector3 operator/(float rhs) const;

	Vector3& operator+=(const Vector3& rhs);
	Vector3& operator-=(const Vector3& rhs);
	Vector3& operator*=(float rhs);
	Vector3& operator/=(float rhs);

	float dot(const Vector3& rhs) const;
	float lengthSq() const;
	float length() const;
};
