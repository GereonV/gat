#ifndef GAT_ALGORITHMS_HPP
#define GAT_ALGORITHMS_HPP

#include "gat.hpp"

namespace gat::algo {

	template<typename T>
	constexpr auto & min(T const & lhs, T const & rhs, auto const &... tail) noexcept {
		if constexpr(sizeof...(tail))
			return min(lhs, min(rhs, tail...));
		return lhs < rhs ? lhs : rhs;
	}

	template<typename It>
	using value_type = decltype(**((It *) nullptr));

	template<typename T> struct ref_less_struct       { using type = T; };
	template<typename T> struct ref_less_struct<T &>  { using type = T; };
	template<typename T> struct ref_less_struct<T &&> { using type = T; };
	template<typename T> using ref_less = typename ref_less_struct<T>::type; // std::remove_reference_t

	template<typename It, auto Deleter = [](It const &) {}>
	struct algorithm_buffer {
		constexpr algorithm_buffer(auto const & it) : begin{it} {}
		template<typename T> constexpr algorithm_buffer(T && it) : begin{static_cast<T &&>(it)} {}
		constexpr ~algorithm_buffer() { Deleter(begin); }
		It begin;
	};

	// Wagner-Fischer
	// [row.begin; row.begin + rhs.size()) must be valid range
	template<typename It, auto Deleter>
	constexpr ref_less<value_type<It>> levenshtein(std::string_view lhs, std::string_view rhs, algorithm_buffer<It, Deleter> const & row) noexcept {
		using Int = ref_less<value_type<It>>;
		if(!lhs.size())
			return rhs.size();
		if(!rhs.size())
			return lhs.size();
		auto it = row.begin;
		for(std::size_t i{1}; i <= rhs.size(); ++it)
			*it = ++i;
		Int left;
		for(Int i{}; i < lhs.size();) { // each row
			auto c = lhs[i];
			auto corner = i;
			left = ++i;
			for(it = row.begin; auto c2 : rhs) { // each column
				auto up = *it;
				*it = left = c == c2 ? corner : 1 + min(corner, left, up);
				corner = up;
				++it;
			}
		}
		return left;
	}

	template<typename Int = std::size_t>
	[[nodiscard]] constexpr Int levenshtein(std::string_view lhs, std::string_view rhs) noexcept {
		return levenshtein<Int *, [](auto ptr) { delete[] ptr; }>(lhs, rhs, new Int[rhs.size()]);
	}

}

#endif // GAT_ALGORITHMS_HPP
