
#include "stdafx.h"
#include "Mtx33.h"

using namespace Matrix33;
constexpr float PI_OVER_180 = 0.0174532925199432f;;

Matrix_33::Matrix_33(const float* pArr)
{
	for (size_t i = 0; i < matrix_33.size(); ++i)
	{
		for (size_t j = 0; j < matrix_33[i].size(); ++j)
		{
			matrix_33[i][j] = pArr[i* matrix_33[i].size() +j];
		}
	}
}

Matrix_33::Matrix_33(const Matrix_33& rhs)
{
	for (size_t i = 0; i < matrix_33.size(); ++i)
	{
		for (size_t j = 0; j < matrix_33[i].size(); ++j)
		{
			matrix_33[i][j] = rhs.matrix_33[i][j];
		}
	}
}

// Helper swap function for copy assignment
static void swap(Matrix_33& lhs, Matrix_33& rhs)
{
	// Swapping of all data members
	for (int i = 0; i < lhs.matrix_33.size(); ++i) {
		for (int j = 0; j < lhs.matrix_33[i].size(); ++j) {
			std::swap(lhs(i, j), rhs(i, j));
		}
	}
}


Matrix_33& Matrix_33::operator=(const Matrix_33& rhs)
{
	Matrix_33 temp{ rhs };
	swap(*this, temp);
	return *this;
}

Matrix_33& Matrix33::Matrix_33::operator+=(const Matrix_33& rhs)
{
	matrix_33[0][0] += rhs(0, 0); matrix_33[0][1] += rhs(0, 1); matrix_33[0][2] += rhs(0, 2);
	matrix_33[1][0] += rhs(1, 0); matrix_33[1][1] += rhs(1, 1); matrix_33[1][2] += rhs(1, 2);
	matrix_33[2][0] += rhs(2, 0); matrix_33[2][1] += rhs(2, 1); matrix_33[2][2] += rhs(2, 2);

	return *this;
}

Matrix_33& Matrix33::Matrix_33::operator-=(const Matrix_33& rhs)
{
	matrix_33[0][0] -= rhs(0, 0); matrix_33[0][1] -= rhs(0, 1); matrix_33[0][2] -= rhs(0, 2);
	matrix_33[1][0] -= rhs(1, 0); matrix_33[1][1] -= rhs(1, 1); matrix_33[1][2] -= rhs(1, 2);
	matrix_33[2][0] -= rhs(2, 0); matrix_33[2][1] -= rhs(2, 1); matrix_33[2][2] -= rhs(2, 2);

	return *this;
}

float& Matrix33::Matrix_33::operator()(int row, int col)
{
	return matrix_33[row][col];
}

const float& Matrix33::Matrix_33::operator()(int row, int col) const
{
	return matrix_33[row][col];
}

Matrix_33& Matrix33::Matrix_33::operator*=(const float scale)
{
	for (size_t i = 0; i < matrix_33.size(); ++i)
	{
		for (size_t j = 0; j < matrix_33.size(); ++j)
		{
			matrix_33[i][j] = scale;
		}
	}

	return *this;
}

Matrix_33& Matrix_33::operator*=(const Matrix_33& rhs)
{
	Matrix_33 temp{ *this };

	// First row
	matrix_33[0][0] = temp.matrix_33[0][0] * rhs.matrix_33[0][0] + temp.matrix_33[0][1] * rhs.matrix_33[1][0] + temp.matrix_33[0][2] * rhs.matrix_33[2][0];
	matrix_33[0][1] = temp.matrix_33[0][0] * rhs.matrix_33[0][1] + temp.matrix_33[0][1] * rhs.matrix_33[1][1] + temp.matrix_33[0][2] * rhs.matrix_33[2][1];
	matrix_33[0][2] = temp.matrix_33[0][0] * rhs.matrix_33[0][2] + temp.matrix_33[0][1] * rhs.matrix_33[1][2] + temp.matrix_33[0][2] * rhs.matrix_33[2][2];

	// Second row
	matrix_33[1][0] = temp.matrix_33[1][0] * rhs.matrix_33[0][0] + temp.matrix_33[1][1] * rhs.matrix_33[1][0] + temp.matrix_33[1][2] * rhs.matrix_33[2][0];
	matrix_33[1][1] = temp.matrix_33[1][0] * rhs.matrix_33[0][1] + temp.matrix_33[1][1] * rhs.matrix_33[1][1] + temp.matrix_33[1][2] * rhs.matrix_33[2][1];
	matrix_33[1][2] = temp.matrix_33[1][0] * rhs.matrix_33[0][2] + temp.matrix_33[1][1] * rhs.matrix_33[1][2] + temp.matrix_33[1][2] * rhs.matrix_33[2][2];

	// Third row
	matrix_33[2][0] = temp.matrix_33[2][0] * rhs.matrix_33[0][0] + temp.matrix_33[2][1] * rhs.matrix_33[1][0] + temp.matrix_33[2][2] * rhs.matrix_33[2][0];
	matrix_33[2][1] = temp.matrix_33[2][0] * rhs.matrix_33[0][1] + temp.matrix_33[2][1] * rhs.matrix_33[1][1] + temp.matrix_33[2][2] * rhs.matrix_33[2][1];
	matrix_33[2][2] = temp.matrix_33[2][0] * rhs.matrix_33[0][2] + temp.matrix_33[2][1] * rhs.matrix_33[1][2] + temp.matrix_33[2][2] * rhs.matrix_33[2][2];

	return *this;
}

std::ostream& Matrix33::operator<<(std::ostream& os, const Matrix_33& input)
{
	os << "[" << input(0, 0) << ", " << input(0, 1) << ", " << input(0, 2) << "] " << endl
		<< "[" << input(1, 0) << ", " << input(1, 1) << ", " << input(1, 2) << "] " << endl
		<< "[" << input(2, 0) << ", " << input(2, 1) << ", " << input(2, 2) << "]" << endl;
	return os;
}

Matrix_33 Matrix33::operator*(const Matrix_33& lhs, const Matrix_33& rhs)
{
	Matrix_33 result{ lhs };
	result *= rhs;

	return result;
}

float Matrix33::Matrix_33Determinant(const Matrix_33& input)
{
	float result{};
	result = input(0,0) * (input(1,1) * input(2,2) - input(1, 2) * input(2, 1))
		- input(0,1) * (input(1, 0) * input(2, 2) - input(1, 2) * input(2, 0))
		+ input(0,2) * (input(1, 0) * input(2, 1) - input(1, 1) * input(2, 0));
	return result;
}

void Matrix33::Matrix_33Identitiy(Matrix_33& input)
{
	input(0, 0) = 1.f; input(0, 1) = 0.f; input(0, 2) = 0.f;
	input(1, 0) = 0.f; input(1, 1) = 1.f; input(1, 2) = 0.f;
	input(2, 0) = 0.f; input(2, 1) = 0.f; input(2, 2) = 1.f;
}

void Matrix33::Matrix_33Transpose(Matrix_33& result, const Matrix_33& input)
{
	// Transpose the matrix by swapping rows and columns
	result(0, 0) = input(0, 0);
	result(0, 1) = input(1, 0);
	result(0, 2) = input(2, 0);

	result(1, 0) = input(0, 1);
	result(1, 1) = input(1, 1);
	result(1, 2) = input(2, 1);

	result(2, 0) = input(0, 2);
	result(2, 1) = input(1, 2);
	result(2, 2) = input(2, 2);
}

void Matrix33::Matrix_33Inverse(Matrix_33& result, float& determinant, const Matrix_33& input)
{
	// Calculations of co factor matrix
	Matrix_33 adjoint{};
	adjoint(0, 0) = (input(1, 1) * input(2, 2)) - (input(2, 1) * input(1, 2));
	adjoint(0, 1) = -((input(1, 0) * input(2, 2)) - (input(2, 0) * input(1, 2)));
	adjoint(0, 2) = (input(1, 0) * input(2, 1)) - (input(2, 0) * input(1, 1));

	adjoint(1, 0) = -((input(0, 1) * input(2, 2)) - (input(2, 1) * input(0, 2)));
	adjoint(1, 1) = (input(0, 0) * input(2, 2)) - (input(2, 0) * input(0, 2));
	adjoint(1, 2) = -((input(0, 0) * input(2, 1)) - (input(2, 0) * input(0, 1)));

	adjoint(2, 0) = (input(0, 1) * input(1, 2)) - (input(1, 1) * input(0, 2));
	adjoint(2, 1) = -((input(0, 0) * input(1, 2)) - (input(1, 0) * input(0, 2)));
	adjoint(2, 2) = (input(0, 0) * input(1, 1)) - (input(1, 0) * input(0, 1));

	// Transpose co factor matrix
	Matrix_33 transpose{};
	Matrix_33Transpose(transpose, adjoint);

	// Find determinant using diagonal - anti-diagonal
	determinant = Matrix_33Determinant(input);
	// Checking if determinant not equals to 0, can inverse
	if (determinant != 0)
	{
		// Calculate the inverse matrix
		result(0, 0) = transpose(0, 0) / determinant;
		result(0, 1) = transpose(0, 1) / determinant;
		result(0, 2) = transpose(0, 2) / determinant;

		result(1, 0) = transpose(1, 0) / determinant;
		result(1, 1) = transpose(1, 1) / determinant;
		result(1, 2) = transpose(1, 2) / determinant;

		result(2, 0) = transpose(2, 0) / determinant;
		result(2, 1) = transpose(2, 1) / determinant;
		result(2, 2) = transpose(2, 2) / determinant;
	}
	else {
		// If determinant is 0, set result to 0
		result = Matrix_33();
	}
}

void Matrix33::Matrix_33Scale(Matrix_33& result, float const& x, float const& y)
{
	result(0, 0) = x;    result(0, 1) = 0;    result(0, 2) = 0;
	result(1, 0) = 0;    result(1, 1) = y;    result(1, 2) = 0;
	result(2, 0) = 0;    result(2, 1) = 0;    result(2, 2) = 1; 
}

void Matrix33::Matrix_33Rot(Matrix_33& result, float angle)
{
	result(0, 0) = std::cos(angle);   result(0, 1) = -(std::sin(angle));  result(0, 2) = 0;
	result(1, 0) = std::sin(angle);   result(1, 1) = std::cos(angle);   result(1, 2) = 0;
	result(2, 0) = 0;          result(2, 1) = 0;          result(2, 2) = 1;
}

void Matrix33::Matrix_33RotDeg(Matrix_33& result, float angle)
{
	result(0, 0) = std::cos(angle * PI_OVER_180);   result(0, 1) = -(std::sin(angle * PI_OVER_180));  result(0, 2) = 0;
	result(1, 0) = std::sin(angle * PI_OVER_180);   result(1, 1) = std::cos(angle * PI_OVER_180);   result(1, 2) = 0;
	result(2, 0) = 0;          result(2, 1) = 0;          result(2, 2) = 1;
}

void Matrix33::Matrix_33Translate(Matrix_33& result, float const& x, float const& y)
{
	result(0, 0) = 1;    result(0, 1) = 0;    result(0, 2) = x;
	result(1, 0) = 0;    result(1, 1) = 1;    result(1, 2) = y;
	result(2, 0) = 0;    result(2, 1) = 0;    result(2, 2) = 1;
}

void Matrix33::Matrix_33TranslateApply(Matrix_33& result, const Matrix_33& input, float const& x, float const& y)
{
	// Create a Translate matrix
	Matrix_33 translate_matrix{};
	Matrix_33Scale(translate_matrix, x, y);

	// Initialize result to zero
	result = Matrix_33();

	// Apply the translation to the input matrix
	result(0, 0) = input(0, 0) * translate_matrix(0, 0) + input(0, 1) * translate_matrix(1, 0) + input(0, 2) * translate_matrix(2, 0);
	result(0, 1) = input(0, 0) * translate_matrix(0, 1) + input(0, 1) * translate_matrix(1, 1) + input(0, 2) * translate_matrix(2, 1);
	result(0, 2) = input(0, 0) * translate_matrix(0, 2) + input(0, 1) * translate_matrix(1, 2) + input(0, 2) * translate_matrix(2, 2);

	result(1, 0) = input(1, 0) * translate_matrix(0, 0) + input(1, 1) * translate_matrix(1, 0) + input(1, 2) * translate_matrix(2, 0);
	result(1, 1) = input(1, 0) * translate_matrix(0, 1) + input(1, 1) * translate_matrix(1, 1) + input(1, 2) * translate_matrix(2, 1);
	result(1, 2) = input(1, 0) * translate_matrix(0, 2) + input(1, 1) * translate_matrix(1, 2) + input(1, 2) * translate_matrix(2, 2);

	result(2, 0) = input(2, 0) * translate_matrix(0, 0) + input(2, 1) * translate_matrix(1, 0) + input(2, 2) * translate_matrix(2, 0);
	result(2, 1) = input(2, 0) * translate_matrix(0, 1) + input(2, 1) * translate_matrix(1, 1) + input(2, 2) * translate_matrix(2, 1);
	result(2, 2) = input(2, 0) * translate_matrix(0, 2) + input(2, 1) * translate_matrix(1, 2) + input(2, 2) * translate_matrix(2, 2);
}

void Matrix33::Matrix_33SetVector2Col(Vector2& result, const Matrix_33& input, unsigned int const& col)
{
	result.x = input(0, col);
	result.y = input(1, col);
}

void Matrix33::Matrix_33SetVector2Row(Vector2& result, const Matrix_33& input, unsigned int const& row)
{
	result.x = input(row, 0);
	result.y = input(row, 1);
}

void Matrix33::Matrix_33ScaleApply(Matrix_33& result, const Matrix_33& input, float const& x, float const& y)
{
	// Create a scaling matrix
	Matrix_33 scale_matrix{};
	Matrix_33Scale(scale_matrix, x, y);

	// Initialize result to zero
	result = Matrix_33();

	// Multiply input matrix by the scaling matrix
	result(0, 0) = input(0, 0) * scale_matrix(0, 0) + input(0, 1) * scale_matrix(1, 0) + input(0, 2) * scale_matrix(2, 0);
	result(0, 1) = input(0, 0) * scale_matrix(0, 1) + input(0, 1) * scale_matrix(1, 1) + input(0, 2) * scale_matrix(2, 1);
	result(0, 2) = input(0, 0) * scale_matrix(0, 2) + input(0, 1) * scale_matrix(1, 2) + input(0, 2) * scale_matrix(2, 2);

	result(1, 0) = input(1, 0) * scale_matrix(0, 0) + input(1, 1) * scale_matrix(1, 0) + input(1, 2) * scale_matrix(2, 0);
	result(1, 1) = input(1, 0) * scale_matrix(0, 1) + input(1, 1) * scale_matrix(1, 1) + input(1, 2) * scale_matrix(2, 1);
	result(1, 2) = input(1, 0) * scale_matrix(0, 2) + input(1, 1) * scale_matrix(1, 2) + input(1, 2) * scale_matrix(2, 2);

	result(2, 0) = input(2, 0) * scale_matrix(0, 0) + input(2, 1) * scale_matrix(1, 0) + input(2, 2) * scale_matrix(2, 0);
	result(2, 1) = input(2, 0) * scale_matrix(0, 1) + input(2, 1) * scale_matrix(1, 1) + input(2, 2) * scale_matrix(2, 1);
	result(2, 2) = input(2, 0) * scale_matrix(0, 2) + input(2, 1) * scale_matrix(1, 2) + input(2, 2) * scale_matrix(2, 2);
}

Vector2 Matrix33::operator*(const Vector2& lhs, const Matrix_33& rhs)
{
	Vector2 result{};
	result.x = rhs(0, 0) * lhs.x + rhs(0, 1) * lhs.y + rhs(0, 2);
	result.y = rhs(1, 0) * lhs.x + rhs(1, 1) * lhs.y + rhs(1, 2);

	return result;
}

Vector2 Matrix33::operator*(const Matrix_33& lhs, const Vector2& rhs)
{
	Vector2 result{};
	result.x = lhs(0, 0) * rhs.x + lhs(0, 1) * rhs.y + lhs(0, 2);
	result.y = lhs(1, 0) * rhs.x + lhs(1, 1) * rhs.y + lhs(1, 2);

	return result;
}
