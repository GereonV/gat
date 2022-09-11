#ifndef _GAT_COMBINATORS_HPP_
#define _GAT_COMBINATORS_HPP_

#include <optional>
#include <tuple>
#include <vector>
#include "gat.hpp"

#define COMB(name, type) constexpr inline parser<type> name = [](std::string_view sv) noexcept -> result<type>

namespace gat::combinators {

    //              many<Parser    a   > = Parser [a] # always
    //              some<Parser    a   > = Parser [a]
    //            choice<Parser... a   > = Parser a
    //          optional<Parser    a   > = Parser a?  # always
    //          sequence<Parser    a...> = Parser a...
    //          min<Int><Parser    a   > = Parser [a]
    //       map<a -> b><Parser    a   > = Parser b
    //      exactly<Int><Parser    a   > = Parser a[]
    //    left<Parser a><Parser    b   > = Parser a
    //   right<Parser a><Parser    b   > = Parser b
    //   ahead<Parser a><Parser    b   > = Parser a
    // between<Int><Int><Parser    a   > = Parser [a]

    template<auto p>
    COMB(many, std::vector<result_type_t<p>>) {
        std::vector<result_type_t<p>> res;
        for(;;) {
            auto r = p(sv);
            if(!r)
                return {sv, std::move(res)};
            res.push_back(std::move(r.value));
            sv = r.remaining;
        }
    };

    template<auto p>
    COMB(some, std::vector<result_type_t<p>>) {
        auto r = p(sv);
        if(!r)
            return {};
        std::vector<result_type_t<p>> res{r};
        for(;;) {
            r = p(sv = r.remaining);
            if(!r)
                return {sv, std::move(res)};
            res.push_back(std::move(r.value));
        }
    };

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

    template<std::size_t I, typename T>
    struct tuple_leaf {
        T value;
    };

    template<std::size_t I, typename... T>
    struct tuple_impl;

    template<std::size_t I>
    struct tuple_impl<I>{};

    template<std::size_t I, typename Head, typename... Tail>
    struct tuple_impl<I, Head, Tail...> : tuple_leaf<I, Head>, tuple_impl<I + 1, Tail...> {};

    template<typename... Types>
    using tuple = tuple_impl<0, Types...>;

    template<auto... ps>
    COMB(sequence, std::tuple<result_type_t<ps>...>) {
        static_assert(sizeof...(ps) >= 2);
        // TODO
        // maybe recursive
    };

}

#undef COMB

#endif // _GAT_COMBINATORS_HPP_
