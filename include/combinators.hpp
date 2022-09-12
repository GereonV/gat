#ifndef _GAT_COMBINATORS_HPP_
#define _GAT_COMBINATORS_HPP_

#include <optional>
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
    // sentinel<Parser a>Parser    b   > = Parser a
    // between<Int><Int><Parser    a   > = Parser [a]

    template<std::size_t N, auto p>
    COMB(min, std::vector<result_type_t<p>>) {
        std::vector<result_type_t<p>> res;
        for(;;) {
            auto r = p(sv);
            if(!r) {
                if(res.size() < N)
                    return {};
                return {sv, std::move(res)};
            }
            res.push_back(std::move(r.value));
            sv = r.remaining;
        }
    };

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

    template<auto... ps>
    COMB(sequence, tuple<result_type_t<ps>...>) {
        static_assert(sizeof...(ps));
        return [&sv]<auto p, auto... tail>() -> decltype(sequence<ps...>({})) {
            auto res = p(sv);
            if(!res)
                return {};
            if constexpr(sizeof...(ps) == 1) {
                return {res.remaining, {std::move(res.value)}};
            } else {
                auto res2 = sequence<tail...>(res.remaining);
                if(!res2)
                    return {};
                return {res2.remaining, {std::move(res.value), std::move(res2.value)}};
            }
        }.template operator()<ps...>();
    };

}

#undef COMB

#endif // _GAT_COMBINATORS_HPP_
