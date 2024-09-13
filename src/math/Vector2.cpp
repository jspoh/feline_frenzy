/*****************************************************************//**
 * \file   Vector2.cpp
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#include "stdafx.h"
#include "Vector2.h"

Vector2 Vector2::operator+(const Vector2& rhs) const {
	return Vector2(x + rhs.x, y + rhs.y);
}

Vector2 Vector2::operator-(const Vector2& rhs) const {
	return Vector2(x - rhs.x, y - rhs.y);
}

Vector2 Vector2::operator*(float rhs) const {
	return Vector2(x * rhs, y * rhs);
}

Vector2 Vector2::operator/(float rhs) const {
	return Vector2(x / rhs, y / rhs);
}

Vector2& Vector2::operator+=(const Vector2& rhs) {
	x += rhs.x;
	y += rhs.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}

Vector2& Vector2::operator*=(float rhs) {
	x *= rhs;
	y *= rhs;
	return *this;
}

Vector2& Vector2::operator/=(float rhs) {
	x /= rhs;
	y /= rhs;
	return *this;
}

float Vector2::dot(const Vector2& rhs) const {
	return x * rhs.x + y * rhs.y;
}

float Vector2::length() const {
	return sqrt(lengthSq());
}

float Vector2::lengthSq() const {
	return x * x + y * y;
}

Vector2 Vector2::normalized() const {
	float len = length();
	return Vector2(x / len, y / len);
}

Vector2 Vector2::normalize() {
	float len = length();
	x /= len;
	y /= len;
	return *this;
}
