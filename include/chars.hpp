#ifndef _GAT_CHARS_HPP_
#define _GAT_CHARS_HPP_

#include "gat.hpp"

#define PARSER(name) constexpr inline parser<char> name = [](std::string_view sv) noexcept -> std::vector<result<char>>
#define BETWEEN(low, high) (low <= c && c <= high)
#define PARSE_ONE_IF(expr) {\
    if(sv.empty())\
        return {};\
    auto c = sv[0];\
    if(!(expr))\
        return {};\
    sv.remove_prefix(1);\
    return {{sv, c}};\
}

namespace gat::chars {

    // Parser Char:
    //               end = [\0]
    //               one = [^]
    //            bdigit = [01]
    //             digit = [0-9]
    //            xdigit = [0-9A-Za-z]
    //         uppercase = [A-Z]
    //         lowercase = [a-z]
    //            letter = [A-Za-z]
    //              word = [A-Za-z0-9_]
    //               dot = [^\n\r]
    //        whitespace = [ \n\r\t\v\f]
    //       exact<Char> = [%]
    //      set<Char...> = [%...]
    //    any_of<Char[]> = [%]
    //  inv<Parser Char> = [^$]
    // range<Char, Char> = [$1-$2]

    // string<Char[]> = Parser StringView
    
    template<char match>
    PARSER(exact) PARSE_ONE_IF(c == match);

    template<char... matches>
    PARSER(set) PARSE_ONE_IF((c == matches) || ...);

    template<literal str>
    PARSER(any_of) PARSE_ONE_IF(sv.find_first_of(str.value) != sv.npos);

    template<parser<char> p>
    PARSER(inv) {
        auto res = p(sv);
        if(!res.empty())
            return {};
        auto c = sv[0];
        sv.remove_prefix(1);
        return {{sv, c}};
    };

    template<char low, char high>
    PARSER(range) PARSE_ONE_IF(BETWEEN(low, high));

    PARSER(end) {
        if(!sv.empty())
            return {};
        return {{sv, 0}};
    };

    PARSER(one) PARSE_ONE_IF(true);
    PARSER(bdigit) PARSE_ONE_IF(BETWEEN('0', '1'));
    PARSER(digit) PARSE_ONE_IF(BETWEEN('0', '9'));
    PARSER(xdigit) PARSE_ONE_IF(BETWEEN('0', '9') || BETWEEN('A', 'F') || BETWEEN('a', 'z'));
    PARSER(uppercase) PARSE_ONE_IF(BETWEEN('A', 'Z'));
    PARSER(lowercase) PARSE_ONE_IF(BETWEEN('a', 'z'));
    PARSER(letter) PARSE_ONE_IF(BETWEEN('A', 'Z') || BETWEEN('a', 'z'));
    PARSER(word) PARSE_ONE_IF(BETWEEN('A', 'Z') || BETWEEN('a', 'z') || BETWEEN('0', '9') || c == '_');
    PARSER(dot) PARSE_ONE_IF(c != '\n' && c != '\r');
    constexpr inline auto whitespace = any_of<" \n\r\t\v\f">;

    template<literal str>
    constexpr inline parser<std::string_view> string = [](std::string_view sv) noexcept -> std::vector<result<std::string_view>> {
        if(!sv.starts_with(str.value))
            return {};
        auto chars = sizeof(str.value) - 1;
        sv.remove_prefix(chars);
        return {{sv, {str.value, chars}}};
    };

}

#undef PARSE_ONE_IF
#undef BETWEEN
#undef PARSER

#endif // _GAT_CHARS_HPP_
