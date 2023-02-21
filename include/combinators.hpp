#ifndef _GAT_COMBINATORS_HPP_
#define _GAT_COMBINATORS_HPP_

#include <optional>
#include <utility>
#include <vector>
#include "parse.hpp"

#define COMMA ,
#define COMB(name, type) constexpr inline parser<type> name = [](std::string_view sv) noexcept -> result<type>

namespace gat::combinators {

	//            choice<Parser... a   > = Parser a
	//          optional<Parser    a   > = Parser a?  # always
	//          sequence<Parser    a...> = Parser {a...}
	//          min<Int><Parser    a   > = Parser [a]
	//       map<a -> b><Parser    a   > = Parser b
	//      exactly<Int><Parser    a   > = Parser a[]
	//    left<Parser a><Parser    b   > = Parser a
	//   right<Parser a><Parser    b   > = Parser b
	//    both<Parser a><Parser    b   > = Parser {a, b}
	//   ahead<Parser a><Parser    b   > = Parser a
	// between<Int><Int><Parser    a   > = Parser [a]

	template<auto p, auto... ps>
	COMB(choice, result_type_t<p>) {
		static_assert(sizeof...(ps));
		auto res = p(sv);
		if(!res)
			((res = ps(sv)) || ...);
		return res;
	};

	template<auto p>
	COMB(optional, std::optional<result_type_t<p>>) {
		auto res = p(sv);
		if(!res)
			return {sv, {}};
		return {res.remaining, std::move(res.value)};
	};

	template<auto p, auto... ps>
	COMB(sequence, tuple<result_type_t<p> COMMA result_type_t<ps>...>) {
		auto res = p(sv);
		if(!res)
			return {};
		if constexpr(sizeof...(ps)) {
			auto res2 = sequence<ps...>(res.remaining);
			if(!res2)
				return {};
			return {res2.remaining, {std::move(res.value), std::move(res2.value)}};
		} else {
			return {res.remaining, {std::move(res.value)}};
		}
	};

	template<std::size_t N, auto p>
	COMB(min, std::vector<result_type_t<p>>) {
		std::vector<result_type_t<p>> res;
		res.reserve(N);
		for(;;) {
			auto r = p(sv);
			if(!r)
				break;
			res.push_back(std::move(r.value));
			sv = r.remaining;
		}
		if(res.size() < N)
			return {};
		return {sv, std::move(res)};
	};

	template<auto f, auto p>
	COMB(map, decltype(f(result_type_t<p>{}))) {
		auto res = p(sv);
		if(!res)
			return {};
		return {res.remaining, f(std::move(res.value))};
	};

	template<std::size_t N, auto p>
	COMB(exactly, result_type_t<p>[N]) {
		result<result_type_t<p>[N]> res;
		res.error = false;
		res.remaining = sv;
		for(std::size_t i{}; i < N; ++i) {
			auto r = p(res.remaining);
			if(!r)
				return {};
			res.value[i] = std::move(r.value);
			res.remaining = r.remaining;
		}
		return res;
	};

	template<auto l, auto r>
	COMB(left, result_type_t<l>) {
		auto res = l(sv);
		if(!res)
			return {};
		auto res2 = r(res.remaining);
		if(!res2)
			return {};
		return {res2.remaining, std::move(res.value)};
	};

	template<auto l, auto r>
	COMB(right, result_type_t<r>) {
		auto res = l(sv);
		if(!res)
			return {};
		return r(res.remaining);
	};

	template<auto l, auto r>
	COMB(both, std::pair<result_type_t<l> COMMA result_type_t<r>>) {
		auto end = sv.end();
		for(std::size_t len{}; len <= sv.size(); ++len) {
			auto res = l({sv.data(), len});
			if(!res)
				continue;
			auto res2 = r({res.remaining.begin(), end});
			if(res2)
				return {res2.remaining, {std::move(res.value), std::move(res2.value)}};
		}
		return {};
	};

	template<auto l, auto r>
	COMB(ahead, result_type_t<l>) {
		auto end = sv.end();
		for(std::size_t len{}; len <= sv.size(); ++len) {
			auto res = l({sv.data(), len});
			if(!res)
				continue;
			auto res2 = r({res.remaining.begin(), end});
			if(res2)
				return res;
		}
		return {};
	};

	template<std::size_t Min, std::size_t Max, auto p>
	COMB(between, std::vector<result_type_t<p>>) {
		static_assert(Min <= Max);
		std::vector<result_type_t<p>> res;
		res.reserve(Min);
		while(res.size() < Max) {
			auto r = p(sv);
			if(!r)
				break;
			res.push_back(std::move(r.value));
			sv = r.remaining;
		}
		if(res.size() < Min)
			return {};
		return {sv, std::move(res)};
	};

}

#undef COMB
#undef COMMA

#endif // _GAT_COMBINATORS_HPP_
