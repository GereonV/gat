#include <iostream>
#include <string_view>
#include "combinators.hpp"

int main() {
    using namespace gat::combinators;
    using namespace std::literals;
    static_assert(chars::sequence<chars::letter, chars::digit>("a0"));
    static_assert(some<chars::box<chars::letter>>("abc123"sv));
    std::cout << "Hello World!\n";
}
