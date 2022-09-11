#ifndef _GAT_GAT_HPP_
#define _GAT_GAT_HPP_

#include <string_view>

namespace gat {

    // Result a = (a, StringView)
    template<typename ResultType>
    struct result {
        result() = default;
        template<typename R>
        constexpr result(std::string_view sv, R && res) noexcept
            : remaining{sv}, value{static_cast<R &&>(res)}, error{} {}
        constexpr result(std::string_view sv, ResultType const & res) noexcept
            : remaining{sv}, value{res}, error{} {}
        constexpr operator bool() const noexcept { return !error; }
        std::string_view remaining;
        ResultType value;
        bool error{true};
    };

    // Parser a = StringView -> Result a
    template<typename ResultType>
    using parser = result<ResultType>(*)(std::string_view) noexcept;

    template<auto p>
    using result_type_t = decltype(p({}).result);

    template<auto p>
    using result_t = result<result_type_t<p>>;

    template<auto p>
    using parser_t = parser<result_type_t<p>>;

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
