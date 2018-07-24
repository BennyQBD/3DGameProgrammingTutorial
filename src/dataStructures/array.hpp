#pragma once

#include <vector>

template<typename T>
class Array : public std::vector<T>
{
public:
	Array<T>() : std::vector<T>() {}
	Array<T>(size_t n) : std::vector<T>(n) {}

	void swap_remove(size_t index)
	{
		std::swap((*this)[index], (*this)[this->size()-1]);
		this->pop_back();
	}
};

