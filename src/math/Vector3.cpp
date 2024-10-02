/*****************************************************************//**
 * \file   Vector3.cpp
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"

Vector3 Vector3::operator+(const Vector3& rhs) const {
	return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vector3 Vector3::operator-(const Vector3& rhs) const {
	return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vector3 Vector3::operator*(float rhs) const {
	return Vector3(x * rhs, y * rhs, z * rhs);
}

// Matrix33 multiplication
Vector3 Vector3::operator*(const Matrix33::Matrix_33& mat) const {
	Vector3 result;
	result.x = mat.matrix_33[0][0] * x +  mat.matrix_33[0][1] * y +  mat.matrix_33[0][2] * z;
	result.y = mat.matrix_33[1][0] * x +  mat.matrix_33[1][1] * y +  mat.matrix_33[1][2] * z;
	result.z =  mat.matrix_33[2][0] * x +  mat.matrix_33[2][1] * y +  mat.matrix_33[2][2] * z;
	return result;
}

Vector3 Vector3::operator/(float rhs) const {
	return Vector3(x / rhs, y / rhs, z / rhs);
}

Vector3& Vector3::operator+=(const Vector3& rhs) {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}

Vector3& Vector3::operator*=(float rhs) {
	x *= rhs;
	y *= rhs;
	z *= rhs;
	return *this;
}

Vector3& Vector3::operator/=(float rhs) {
	x /= rhs;
	y /= rhs;
	z /= rhs;
	return *this;
}

float Vector3::dot(const Vector3& rhs) const {
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

float Vector3::length() const {
	return sqrt(lengthSq());
}

float Vector3::lengthSq() const {
	return x * x + y * y + z * z;
}

Vector3 Vector3::normalized() const {
	float len = length();
	return Vector3(x / len, y / len, z / len);
}

Vector3 Vector3::normalize() {
	float len = length();
	x /= len;
	y /= len;
	z /= len;
	return *this;
}
