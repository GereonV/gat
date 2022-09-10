namespace gat::combinators {

    template<decltype(sizeof(char)) N>
    struct literal {
        constexpr literal(char const (&str)[N]) noexcept {
            for(auto i = N; i--;)
                value[i] = str[i];
        }

        char value[N];
    };

    template<typename It>
    struct result {
        result() = default;
        constexpr result(It const & it) noexcept : valid{true}, next{it} {}
        constexpr operator bool() const noexcept { return valid; }

        bool valid{};
        It next;
    };

    template<typename It>
    using parser_t = result<It> (*)(It) noexcept;

    template<typename It, auto parser>
    concept iterator_for = requires() { (parser_t<It>) parser; };

    template<auto... parsers>
    constexpr inline auto sequence = [](auto begin) noexcept {
        result res{begin};
        ((res = parsers(res.next)) && ...);
        return res;
    };

    template<literal target>
    constexpr inline auto string = []<typename It>(It it) noexcept -> result<It> {
        for(auto ptr = target.value; *ptr; ++ptr, ++it)
            if(*it != *ptr)
                return {};
        return it;
    };

    // n-times & n-k-times

    template<auto parser>
    struct many_fn {
        constexpr auto operator()(iterator_for<parser> auto begin, auto const & end) const noexcept {
            result res{begin};
            while(begin != end) {
                auto n = parser(res.next);
                if(!n)
                    break;
                res = n;
            }
            return res;
        }

        constexpr auto operator()(auto && r) const noexcept { return this->operator()(begin(r), end(r)); }
    };

    template<auto parser>
    constexpr inline many_fn<parser> many;

    template<auto parser>
    struct some_fn {
        constexpr auto operator()(iterator_for<parser> auto begin, auto const & end) const noexcept {
            auto first = parser(begin);
            return first ? many<parser>(first.next, end) : first;
        }

        constexpr auto operator()(auto && r) const noexcept { return this->operator()(begin(r), end(r)); }
    };

    template<auto parser>
    constexpr inline some_fn<parser> some;

    namespace chars {

        using parser_t = bool (*)(char) noexcept;

        template<char low, char high>
        constexpr bool between(char c) noexcept {
            return low <= c && c <= high;
        };

        template<parser_t... parser>
        constexpr bool any(char c) noexcept {
            return (parser(c) || ...);
        }

        constexpr inline parser_t
                       digit = between<'0', '9'>,
                      bdigit = between<'0', '1'>,
                      xdigit = any<digit, between<'A', 'F'>, between<'a', 'f'>>,
            lowercase_letter = between<'a', 'z'>,
            uppercase_letter = between<'A', 'Z'>,
                      letter = any<lowercase_letter, uppercase_letter>;

        template<parser_t parser>
        constexpr inline auto box = []<typename It>(It begin) noexcept -> result<It> {
            if(parser(*begin))
                return ++begin;
            return {};
        };

        template<parser_t... parsers>
        constexpr inline auto sequence = combinators::sequence<box<parsers>...>;

    }

}
