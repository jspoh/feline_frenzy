#pragma once

#ifndef MTX33
#define MTX33


#include "Vector2.h"

// Foward declaration
class Vector3;

namespace Matrix33 {
	class Matrix_33 {
	public:
		// Ctors
		Matrix_33() : matrix_33{} {}
		Matrix_33(const float* p_arr);
		Matrix_33(const Matrix_33& rhs);
		~Matrix_33() = default;

		// Copy assignment
		Matrix_33& operator=(const Matrix_33& rhs);

		// Operator overloads
		Matrix_33& operator+=(const Matrix_33& rhs);
		Matrix_33& operator-=(const Matrix_33& rhs);
		Matrix_33& operator*=(const float scale);
		Matrix_33& operator*=(const Matrix_33& rhs);

		// Gettors/Settors
		float& operator()(int row, int col);
		const float& operator()(int row, int col) const; 

		// Debug :D
		friend std::ostream& operator<<(std::ostream& os, const Matrix_33& input);
	public:
		std::array<std::array<float, 3>, 3> matrix_33{};

	};

	// Matrix Multiplication (THIS IS SAME AS MATRIX CONCAT)
	Matrix_33 operator* (const Matrix_33& lhs, const Matrix_33& rhs);

	Vector2 operator* (const Vector2& lhs, const Matrix_33& rhs);
	Vector2 operator* (const Matrix_33& lhs, const Vector2& rhs);

	// Matrix stuff

	// Pass in matrix to be used to calculate the determinant from the matrix
	float Matrix_33Determinant(const Matrix_33& input);
	// Result will be the matrix that is identity matrix
	void Matrix_33Identitiy(Matrix_33& result);
	// Result is the matrix that is transposed from input
	void Matrix_33Transpose(Matrix_33& result, const Matrix_33& input);
	// Result is the matrix that is inversed from input
	void Matrix_33Inverse(Matrix_33& result, float& determinant, const Matrix_33& input);
	// Transpose the inverse matrix, result will be the transposed inverse input matrix
	// void Matrix_33InverseTranspose(Matrix_33& result, const Matrix_33& input);
	// Result is the scalar matrix
	void Matrix_33Scale(Matrix_33& result, float const& x, float const& y);
	// Set result to the multiplication of scaling matrix of x and y with input.
	void Matrix_33ScaleApply(Matrix_33& result, const Matrix_33& input, float const& x, float const& y);
	// Result is the rotation matrix
	void Matrix_33Rot(Matrix_33& result, float angle);
	// Result is the rotation matrix in degrees
	void Matrix_33RotDeg(Matrix_33& result, float angle);
	// Translate Matrix, result will be the translated matrix
	void Matrix_33Translate(Matrix_33& result, float const& x, float const& y);
	// Set result to the multiplication of translation matrix of x and y with input.
	void Matrix_33TranslateApply(Matrix_33& result, const Matrix_33& input, float const& x, float const& y);
	// Set vector2 result with first and second elem of selected column from input matrix
	void Matrix_33SetVector2Col(Vector2& result, const Matrix_33& input, unsigned int const& col);
	// Set vector2 result with first and second elem of selected row from input matrix
	void Matrix_33SetVector2Row(Vector2& result, const Matrix_33& input, unsigned int const& row);
}


#endif