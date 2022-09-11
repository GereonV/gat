#ifndef _GAT_COMBINATORS_HPP_
#define _GAT_COMBINATORS_HPP_

#include <vector>
#include "gat.hpp"

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
    constexpr inline parser<std::vector<result_type_t<p>>> many = [](std::string_view sv) noexcept -> result<std::vector<result_type_t<p>>> {
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
    constexpr inline parser<std::vector<result_type_t<p>>> some = [](std::string_view sv) noexcept -> result<std::vector<result_type_t<p>>> {
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
    constexpr inline parser<result_type_t<p>> choice = [](std::string_view sv) noexcept -> result<result_type_t<p>> {
        static_assert(sizeof...(ps));
        auto res = p(sv);
        if(!res)
            ((res = ps(sv)) || ...);
        return res;
    };

}

#endif // _GAT_COMBINATORS_HPP_
