#pragma once


template <typename T>
class Array2D
{
private:
	size_t h;
	size_t w;
	T *arr;
	Array2D(const Array2D&);
	Array2D& operator = (const Array2D&);
public:
	Array2D(size_t x, size_t y): h(x), w(y), arr(new T[x*y]){}
	~Array2D() { delete [] arr;}

	const T& operator () (size_t x, size_t y) const { return arr[y*w+x]; }
	T& operator () (size_t x, size_t y) { return arr[y*w+x]; }	
};











