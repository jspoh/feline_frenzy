/*****************************************************************//**
 * \file   Vector4.cpp
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "../headers/Core/stdafx.h"

Vector4 Vector4::operator+(const Vector4& rhs) const {
	return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

Vector4 Vector4::operator-(const Vector4& rhs) const {
	return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

Vector4 Vector4::operator*(float rhs) const {
	return Vector4(x * rhs, y * rhs, z * rhs, w * rhs);
}

Vector4 Vector4::operator/(float rhs) const {
	return Vector4(x / rhs, y / rhs, z / rhs, w / rhs);
}

Vector4& Vector4::operator+=(const Vector4& rhs) {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	w += rhs.w;
	return *this;
}

Vector4& Vector4::operator-=(const Vector4& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	w -= rhs.w;
	return *this;
}

Vector4& Vector4::operator*=(float rhs) {
	x *= rhs;
	y *= rhs;
	z *= rhs;
	w *= rhs;
	return *this;
}

Vector4& Vector4::operator/=(float rhs) {
	x /= rhs;
	y /= rhs;
	z /= rhs;
	w /= rhs;
	return *this;
}

float Vector4::dot(const Vector4& rhs) const {
	return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
}

float Vector4::length() const {
	return sqrt(lengthSq());
}

float Vector4::lengthSq() const {
	return x * x + y * y + z * z + w * w;
}

Vector4 Vector4::normalized() const {
	float len = length();
	return Vector4(x / len, y / len, z / len, w / len);
}

Vector4 Vector4::normalize() {
	float len = length();
	x /= len;
	y /= len;
	z /= len;
	w /= len;
	return *this;
}
