/*****************************************************************//**
 * \file   Vector3.cpp
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

template<typename T>
inline Vector3<T> Vector3<T>::operator-(const Vector3<T>& rhs) const {
	return Vector3<T>(x - rhs.x, y - rhs.y, z - rhs.z);
}

template<typename T>
inline Vector3<T> Vector3<T>::operator*(typename Vector3<T>::type rhs) const {
	return Vector3<T>(x * rhs, y * rhs, z * rhs);
}

// Matrix33 multiplication
template<typename T>
inline Vector3<T> Vector3<T>::operator*(const Matrix_33& mat) const {
	Vector3<T> result;
	result.x = mat.matrix_33[0][0] * x + mat.matrix_33[0][1] * y + mat.matrix_33[0][2] * z;
	result.y = mat.matrix_33[1][0] * x + mat.matrix_33[1][1] * y + mat.matrix_33[1][2] * z;
	result.z = mat.matrix_33[2][0] * x + mat.matrix_33[2][1] * y + mat.matrix_33[2][2] * z;
	return result;
}

template<typename T>
inline Vector3<T> Vector3<T>::operator/(typename Vector3<T>::type rhs) const {
	return Vector3<T>(x / rhs, y / rhs, z / rhs);
}

template<typename T>
inline Vector3<T>& Vector3<T>::operator+=(const Vector3<T>& rhs) {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}

template<typename T>
inline Vector3<T>& Vector3<T>::operator-=(const Vector3<T>& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}

template<typename T>
Vector3<T>& Vector3<T>::operator*=(typename Vector3<T>::type rhs) {
	x *= rhs;
	y *= rhs;
	z *= rhs;
	return *this;
}

template<typename T>
inline Vector3<T>& Vector3<T>::operator/=(typename Vector3<T>::type rhs) {
	x /= rhs;
	y /= rhs;
	z /= rhs;
	return *this;
}

template<typename T>
inline bool Vector3<T>::operator==(const Vector3<T>& rhs) const {
	return (this->x == rhs.x) && (this->y == rhs.y) && (this->z == rhs.z);
}

template<typename T>
inline bool Vector3<T>::operator!=(const Vector3<T>& rhs) const {
	return (this->x != rhs.x) || (this->y != rhs.y) || (this->z != rhs.z);
}

template<typename T>
inline typename Vector3<T>::type Vector3<T>::dot(const Vector3<T>& rhs) const {
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

template<typename T>
inline typename Vector3<T>::type Vector3<T>::length() const {
	return static_cast<typename Vector3<T>::type>(sqrt(lengthSq()));
}

template<typename T>
inline typename Vector3<T>::type Vector3<T>::lengthSq() const {
	return x * x + y * y + z * z;
}

template<typename T>
inline Vector3<T> Vector3<T>::normalized() const {
	typename Vector3<T>::type len = length();
	return Vector3<T>(x / len, y / len, z / len);
}

template<typename T>
inline Vector3<T> Vector3<T>::normalize() {
	typename Vector3<T>::type len = length();
	x /= len;
	y /= len;
	z /= len;
	return *this;
}

template<typename T>
nlohmann::json Vector3<T>::toJson() const {
	return	{
			{"x", x},
			{"y", y},
			{"z", z},
	};
}

template<typename T>
void Vector3<T>::fromJson(nlohmann::json const& data) {
	x = data.at("x").get<T>();
	y = data.at("y").get<T>();
	z = data.at("z").get<T>();
}
