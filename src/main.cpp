#include <iostream>
#include "chars.hpp"

constexpr void chars() noexcept {
    using namespace gat::chars;
    static_assert(!end("").empty());
    static_assert(one("").empty());
    static_assert(bdigit("").empty());
    static_assert(!bdigit("0").empty());
    static_assert(!bdigit("1").empty());
    static_assert(bdigit("2").empty());
    static_assert(!letter("a").empty());
    static_assert(!letter("z").empty());
    static_assert(!exact<'/'>("/").empty());
    static_assert(exact<'/'>("\\/").empty());
    static_assert(!set<'/', '%'>("/\\").empty());
    static_assert(!set<'/', '%'>("%\\").empty());
    static_assert(set<'/', '%'>("\\/").empty());
    static_assert(!whitespace(" ").empty());
    static_assert(!whitespace("\t").empty());
    static_assert(inv<word>("_").empty());
    static_assert(!inv<word>("|").empty());
    static_assert(range<'0', '9'>("").empty());
    static_assert(!range<'0', '9'>("0").empty());
    static_assert(string<"test">("Hello World").empty());
    static_assert(!string<"test">("test").empty());
}

int main() {
    chars();
    std::cout << "Hello World!\n";
}
