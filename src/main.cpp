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

constexpr void combinators() noexcept {
    using namespace gat::combinators;
    static_assert(!choice<gat::chars::one, gat::chars::word>(""));
    static_assert(choice<gat::chars::one, gat::chars::end>(""));
    static_assert(optional<gat::chars::one>(""));
    static_assert(optional<gat::chars::one>("x"));
    static_assert(sequence<gat::chars::one>("x"));
    static_assert(sequence<gat::chars::one, gat::chars::one>("xy"));
    static_assert(min<0, gat::chars::one>(""));
    static_assert(min<0, gat::chars::one>("").value.empty());
    static_assert(!min<1, gat::chars::one>(""));
    static_assert(min<1, gat::chars::one>("abc"));
    static_assert(min<1, gat::chars::one>("abc").value.size() == 3);
    static_assert(!min<3, gat::chars::one>(""));
    static_assert(min<3, gat::chars::one>("abc"));
    static_assert(min<3, gat::chars::one>("abc").value.size() == 3);
    static_assert(min<3, gat::chars::one>("abcde"));
    static_assert(min<3, gat::chars::one>("abcde").value.size() == 5);
    static_assert(!exact<3, gat::chars::one>(""));
    static_assert(exact<3, gat::chars::one>("abc"));
    static_assert(exact<3, gat::chars::one>("abcde"));
    static_assert(left<gat::chars::digit, gat::chars::word>("1a"));
    static_assert(left<gat::chars::digit, gat::chars::word>("1a").value == '1');
    static_assert(!left<gat::chars::digit, gat::chars::word>("aa"));
    static_assert(!left<gat::chars::digit, gat::chars::word>("a1"));
    static_assert(right<gat::chars::digit, gat::chars::word>("1a"));
    static_assert(right<gat::chars::digit, gat::chars::word>("1a").value == 'a');
    static_assert(!right<gat::chars::digit, gat::chars::word>("aa"));
    static_assert(!right<gat::chars::digit, gat::chars::word>("a1"));
}

int main() {
    chars();
    combinators();
    std::cout << "Hello World!\n";
}
