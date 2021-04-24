#include "matrix.h"

matrix::matrix() : rowCount(1), columnCount(1)
{
	allocSpace();
}

matrix::matrix(size_t rows, size_t columns) : rowCount(rows), columnCount(columns)
{
	allocSpace();
}

matrix::matrix(const matrix& other) : rowCount(other.rowCount), columnCount(other.columnCount)
{
	allocSpace();

	for (size_t row = 0; row < rowCount; ++row)
		for (size_t col = 0; col < columnCount; ++col)
			p[row][col] = other.p[row][col];
}

matrix::~matrix()
{
	freeSpace();
}

matrix& matrix::operator=(const matrix& m)
{
	if (this == &m)
		return *this;

	if (this->rowCount != m.rowCount || this->columnCount != m.columnCount)
	{
		freeSpace();

		this->rowCount = m.rowCount;
		this->columnCount = m.columnCount;

		allocSpace();
	}

	for (size_t row = 0; row < rowCount; ++row)
		for (size_t col = 0; col < columnCount; ++col)
			this->p[row][col] = m.p[row][col];

	return *this;
}

matrix& matrix::operator+=(const matrix& m)
{
	if (this->rowCount != m.rowCount || this->columnCount != m.columnCount)
		throw std::logic_error("matrices must have same size");

	for (size_t row = 0; row < rowCount; ++row)
		for (size_t col = 0; col < columnCount; ++col)
			this->p[row][col] += m.p[row][col];

	return *this;
}

matrix& matrix::operator-=(const matrix& m)
{
	if (this->rowCount != m.rowCount || this->columnCount != m.columnCount)
		throw std::logic_error("matrices must have same size");

	for (size_t row = 0; row < rowCount; ++row)
		for (size_t col = 0; col < columnCount; ++col)
			this->p[row][col] -= m.p[row][col];

	return *this;
}

matrix matrix::operator!() const
{
	matrix res(this->columnCount, this->rowCount);

	for (size_t row = 0; row < rowCount; ++row)
		for (size_t col = 0; col < columnCount; col++)
			res.p[col][row] = this->p[row][col];

	return res;
}

bool matrix::operator==(const matrix& m)
{
	for (int i = 0; i < rowCount; ++i) {
		for (int j = 0; j < columnCount; ++j) {
			if (p[i][j] != m[i][j]) {
				return false;
			}
		}
	}
	return true;
}

long double*& matrix::operator[](int index) const
{
	return p[index];
}

matrix operator+(const matrix& left, const matrix& right)
{
	if (left.rowCount != right.rowCount || left.columnCount != right.columnCount)
		std::cout << "ERROR! Sum" << std::endl;

	matrix tmp(left);

	return tmp += right;
}

matrix operator-(const matrix& left, const matrix& right)
{
	if (left.rowCount != right.rowCount || left.columnCount != right.columnCount)
		std::cout << "ERROR! Sub" << std::endl;

	matrix tmp(left);

	return tmp -= right;
}

matrix operator*(const matrix& left, const matrix& right)
{
	if (left.columnCount != right.rowCount)
		std::cout << "ERROR! multiply" << std::endl;

	matrix res(left.rowCount, right.columnCount);

	for (size_t row = 0; row < left.rowCount; ++row)
		for (size_t col = 0; col < right.columnCount; ++col)
			for (size_t i = 0; i < left.columnCount; ++i)
				res.p[row][col] += left.p[row][i] * right.p[i][col];

	return res;
}

matrix operator*(const matrix& m, const long double num)
{
	matrix res(m.rowCount, m.columnCount);

	for (size_t row = 0; row < m.rowCount; ++row)
		for (size_t col = 0; col < m.columnCount; ++col)
			res.p[row][col] = m.p[row][col] * num;

	return res;
}

matrix operator*(const long double num, const matrix& m)
{
	matrix res(m.rowCount, m.columnCount);

	for (size_t row = 0; row < m.rowCount; ++row)
		for (size_t col = 0; col < m.columnCount; ++col)
			res.p[row][col] = m.p[row][col] * num;

	return res;
}

std::ostream& operator<<(std::ostream& out, const matrix& m)
{
	const int width = 11;
	for (size_t row = 0; row < m.rowCount; ++row)
	{
		for (size_t col = 0; col < m.columnCount; ++col)
		{
			out << std::left << std::setw(width) << m.p[row][col] << "  ";
		}
		out << std::endl;
	}

	return out;
}

std::istream& operator>>(std::istream& in, matrix& m)
{
	for (size_t row = 0; row < m.rowCount; ++row)
	{
		//std::string line;
		//std::getline(std::cin, line);
		//std::istringstream iss(line);

		for (size_t col = 0; col < m.columnCount; ++col)
			in >> m.p[row][col];
	}

	return in;
}

void matrix::randInit(uint32_t randMax)
{
	for (size_t row = 0; row < rowCount; ++row)
		for (size_t col = 0; col < columnCount; ++col)
			p[row][col] = std::rand() % randMax;
}

size_t matrix::rows() const
{
	return rowCount;
}

size_t matrix::columns() const
{
	return columnCount;
}

void matrix::allocSpace()
{
	p = new long double*[rowCount];
	long double* data = new long double[rowCount * columnCount]{ 0 };

	for (int i = 0; i < rowCount; ++i)
		p[i] = &(data[columnCount * i]);
}

void matrix::freeSpace()
{
	delete[] p;
}
