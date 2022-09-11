#include <iostream>
#include "chars.hpp"
#include "combinators.hpp"

constexpr void chars() noexcept {
    using namespace gat::chars;
    static_assert(end(""));
    static_assert(!one(""));
    static_assert(!bdigit(""));
    static_assert(bdigit("0"));
    static_assert(bdigit("1"));
    static_assert(!bdigit("2"));
    static_assert(letter("a"));
    static_assert(letter("z"));
    static_assert(exact<'/'>("/"));
    static_assert(!exact<'/'>("\\/"));
    static_assert(set<'/', '%'>("/\\"));
    static_assert(set<'/', '%'>("%\\"));
    static_assert(!set<'/', '%'>("\\/"));
    static_assert(whitespace(" "));
    static_assert(whitespace("\t"));
    static_assert(!whitespace("x"));
    static_assert(!inv<word>("_"));
    static_assert(inv<word>("|"));
    static_assert(!range<'0', '9'>(""));
    static_assert(range<'0', '9'>("0"));
    static_assert(!range<'0', '9'>("a"));
    static_assert(!string<"test">("Hello World"));
    static_assert(string<"test">("test"));
}

int main() {
    chars();
    static_assert(gat::combinators::many<gat::chars::one>("").value.empty());
    static_assert(gat::combinators::many<gat::chars::one>("abc").value.size() == 3);
    std::cout << "Hello World!\n";
}
