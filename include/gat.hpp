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
        template<typename U>
        constexpr result(result<U> const & other) noexcept
            : error{other.error} {
            if(error)
                return;
            remaining = other.remaining;
            value = other.value;
        }
        template<typename U>
        constexpr result(result<U> && other) noexcept
            : error{other.error} {
            if(error)
                return;
            remaining = other.remaining;
            value = std::move(other.value);
        }
        constexpr operator bool() const noexcept { return !error; }
        std::string_view remaining;
        ResultType value{};
        bool error{true};
    };

    // Parser a = StringView -> Result a
    template<typename ResultType>
    using parser = result<ResultType>(*)(std::string_view) noexcept;

    template<auto p>
    using result_type_t = decltype(p({}).value);

    template<auto p>
    using result_t = result<result_type_t<p>>;

    template<auto p>
    using parser_t = parser<result_type_t<p>>;

    template<std::size_t N>
    struct literal {
        consteval literal(char const (&str)[N]) noexcept {
            for(auto i = N; i--;)
                value[i] = str[i];
        }

        constexpr auto size() const noexcept { return N; }
        constexpr operator std::string_view() const noexcept { return {value, N - 1}; }
        constexpr std::string_view view() const noexcept { return *this; }

        char value[N];
    };

    // string<Char[]> = Parser StringView
    template<literal str>
    constexpr inline parser<std::string_view> string = [](std::string_view sv) noexcept -> result<std::string_view> {
        if(!sv.starts_with(str.value))
            return {};
        sv.remove_prefix(sizeof(str.value) - 1);
        return {sv, str};
    };

    template<typename T, typename... Tail>
    struct tuple {
        T value;
        tuple<Tail...> tail;
    };

    template<typename T>
    struct tuple<T> {
        T value;
    };

}

#endif // _GAT_GAT_HPP_
