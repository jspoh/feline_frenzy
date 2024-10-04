/*****************************************************************//**
 * \file   Vector3.h
 * \brief  
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   September 2024
 * \All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
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
	Vector3 operator*(const Matrix33::Matrix_33& mat) const;
	Vector3 operator/(float rhs) const;

	Vector3& operator+=(const Vector3& rhs);
	Vector3& operator-=(const Vector3& rhs);
	Vector3& operator*=(float rhs);
	Vector3& operator/=(float rhs);

	float dot(const Vector3& rhs) const;
	float lengthSq() const;
	float length() const;
};
