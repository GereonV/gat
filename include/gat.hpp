#ifndef _GAT_GAT_HPP_
#define _GAT_GAT_HPP_

#include <string_view>
#include <vector>

namespace gat {

    // Result a = (a, StringView)
    template<typename ResultType>
    struct result {
        std::string_view remaining;
        ResultType result;
    };

    // Parser a = StringView -> [Result a]
    template<typename ResultType>
    using parser = std::vector<result<ResultType>>(*)(std::string_view) noexcept;

    template<decltype(sizeof(char)) N>
    struct literal {
        constexpr literal(char const (&str)[N]) noexcept {
            for(auto i = N; i--;)
                value[i] = str[i];
        }

        char value[N];
    };

}

#endif // _GAT_GAT_HPP_
