namespace gat {

    // StringView = (Char *, Int)
    // Parser a = StringView -> [(a, StringView)]

    template<decltype(sizeof(char)) N>
    struct literal {
        constexpr literal(char const (&str)[N]) noexcept {
            for(auto i = N; i--;)
                value[i] = str[i];
        }

        char value[N];
    };

}

