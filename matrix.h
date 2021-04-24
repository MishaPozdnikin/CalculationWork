#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

class matrix
{
public:
	matrix();
	matrix(size_t rows, size_t columns);
	matrix(const matrix& other);
	~matrix();

	matrix& operator=(const matrix& m);
	matrix& operator+=(const matrix& m);
	matrix& operator-=(const matrix& m);
	matrix operator!() const;
	long double*& operator[](int index) const;

	friend matrix operator+(const matrix& left, const matrix& right);
	friend matrix operator-(const matrix& left, const matrix& right);

	friend matrix operator*(const matrix& left, const matrix& right);
	friend matrix operator*(const matrix& m, const long double num);
	friend matrix operator*(const long double num, const matrix& m);

	friend std::ostream& operator<<(std::ostream& out, const matrix& m);
	friend std::istream& operator>>(std::istream& in, matrix& m);

	void randInit(uint32_t randMax = 10);
	size_t rows() const;
	size_t columns() const;

private:
	size_t rowCount, columnCount;
	long double** p;

	void allocSpace();
	void freeSpace();
};
