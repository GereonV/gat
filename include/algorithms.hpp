#ifndef GAT_ALGORITHMS_HPP
#define GAT_ALGORITHMS_HPP

#include "gat.hpp"

namespace gat::algo {

	template<typename T>
	constexpr auto & min(T const & lhs, T const & rhs) noexcept {
		return lhs < rhs ? lhs : rhs;
	}

	// Wagner-Fischer (row-size dependendant on rhs)
	constexpr std::size_t levenshtein(std::string_view lhs, std::string_view rhs) noexcept {
		auto lsize = lhs.size(), rsize = rhs.size();
		if(!lsize)
			return rsize;
		if(!rsize)
			return lsize;
		auto size = rsize + 1;
		auto row = new std::size_t[size];
		for(std::size_t i{}; i < size; ++i)
			row[i] = i;
		for(std::size_t i{}; auto c : lhs) { // each row
			auto corner = i; // iota first column
			row[0] = ++i; // i = row number
			for(std::size_t j{}; auto c2 : rhs) { // each column
				auto up = row[j + 1]; // left = row[j]
				row[j + 1] = c == c2 ? corner : 1 + min(row[j], min(corner, up));
				corner = up;
				++j;
			}
		}
		size = row[rsize];
		delete[] row;
		return size;
	}

}

#endif // GAT_ALGORITHMS_HPP
