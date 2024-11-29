/*****************************************************************//**
 * \file   Vector2.cpp
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

template<typename T>
inline Vector2<T> Vector2<T>::operator+(const Vector2<T>& rhs) const {
	return Vector2(x + rhs.x, y + rhs.y);
}

template<typename T>
inline Vector2<T> Vector2<T>::operator-(const Vector2<T>& rhs) const {
	return Vector2(x - rhs.x, y - rhs.y);
}

template<typename T>
inline Vector2<T> Vector2<T>::operator*(typename Vector2<T>::type rhs) const {
	return Vector2(x * rhs, y * rhs);
}

template<typename T>
inline Vector2<T> Vector2<T>::operator/(typename Vector2<T>::type rhs) const {
	return Vector2(x / rhs, y / rhs);
}

template<typename T>
inline Vector2<T>& Vector2<T>::operator+=(const Vector2<T>& rhs) {
	x += rhs.x;
	y += rhs.y;
	return *this;
}

template<typename T>
inline Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}

template<typename T>
inline Vector2<T>& Vector2<T>::operator*=(typename Vector2<T>::type rhs) {
	x *= rhs;
	y *= rhs;
	return *this;
}

template<typename T>
inline Vector2<T>& Vector2<T>::operator/=(typename Vector2<T>::type rhs) {
	x /= rhs;
	y /= rhs;
	return *this;
}

template<typename T>
inline bool Vector2<T>::operator==(const Vector2<T>& rhs) {
	return (this->x == rhs.x) && (this->y == rhs.y);
}

template<typename T>
inline bool Vector2<T>::operator!=(const Vector2<T>& rhs) {
	return (this->x != rhs.x) || (this->y != rhs.y);
}

template<typename T>
inline typename Vector2<T>::type Vector2<T>::dot(const Vector2<T>& rhs) const {
	return x * rhs.x + y * rhs.y;
}

template<typename T>
inline typename Vector2<T>::type Vector2<T>::length() const {
	return static_cast<typename Vector2<T>::type>(sqrt(lengthSq()));
}

template<typename T>
inline typename Vector2<T>::type Vector2<T>::lengthSq() const {
	return x * x + y * y;
}

template<typename T>
inline Vector2<T> Vector2<T>::normalized() const {
	typename Vector2<T>::type len = length();
	return Vector2(x / len, y / len);
}

template<typename T>
inline Vector2<T> Vector2<T>::normalize() {
	typename Vector2<T>::type len = length();
	x /= len;
	y /= len;
	return *this;
}

template<typename T>
nlohmann::json Vector2<T>::toJson() const {
	return	{
			{"x", x},
			{"y", y}
			};
}

template<typename T>
void Vector2<T>::fromJson(nlohmann::json const& data) {
	x = data.at("x").get<T>();
	y = data.at("y").get<T>();
}
