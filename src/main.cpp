#include <iostream>
#include "chars.hpp"
#include "combinators.hpp"

constexpr void chars() noexcept {
    using gat::string;
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
    static_assert(!map<[](char) -> char const * { return "Test"; }, gat::chars::digit>("map"));
    static_assert(map<[](char) -> char const * { return "Test"; }, gat::chars::one>("map"));
    static_assert(map<[](char) -> char const * { return "Test"; }, gat::chars::one>("map").value == std::string_view{"Test"});
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
    static_assert(!both<min<0, gat::chars::word>, gat::chars::digit>("test_"));
    static_assert(!both<min<0, gat::chars::word>, gat::chars::digit>("test\r\n$123"));
    static_assert(both<min<0, gat::chars::word>, gat::chars::digit>("test1"));
    static_assert(both<min<0, gat::chars::word>, gat::chars::digit>("test1").value == std::pair{std::vector{'t', 'e', 's', 't'}, '1'});
    static_assert(both<min<0, gat::chars::word>, gat::chars::digit>("123test"));
    static_assert(both<min<0, gat::chars::word>, gat::chars::digit>("123test").value == std::pair{std::vector<char>{}, '1'});
    static_assert(both<min<1, gat::chars::word>, gat::chars::digit>("123test"));
    static_assert(both<min<1, gat::chars::word>, gat::chars::digit>("123test").value == std::pair{std::vector{'1'}, '2'});
    static_assert(!between<1, 3, gat::chars::letter>(""));
    static_assert(between<1, 3, gat::chars::letter>("a_1"));
    static_assert(between<1, 3, gat::chars::letter>("a_1").value.size() == 1);
    static_assert(between<1, 3, gat::chars::letter>("abc"));
    static_assert(between<1, 3, gat::chars::letter>("abc").value.size() == 3);
    static_assert(between<1, 3, gat::chars::letter>("abcde"));
    static_assert(between<1, 3, gat::chars::letter>("abcde").value.size() == 3);
}

constexpr void json() noexcept {
    using gat::parser;
    using namespace gat::combinators;
    struct json_value {
        constexpr json_value() noexcept : tag{} {}
        constexpr json_value(bool b) noexcept : b{b}, tag{1} {}
        constexpr json_value(int i) noexcept : i{i}, tag{2} {}
        constexpr json_value(std::string_view s) noexcept : s{new std::string{s}}, tag{3} {}
        constexpr json_value(std::vector<json_value> a) noexcept : a{a}, tag{4} {}
        constexpr json_value(std::vector<std::pair<std::string, json_value>> o) noexcept : o{o}, tag{5} {}
        constexpr json_value(json_value const & other) noexcept { *this = other; }
        constexpr json_value(json_value && other) noexcept { *this = std::move(other); }
        constexpr json_value & operator=(json_value const & other) noexcept {
            tag = other.tag;
            switch(tag) {
            case 1:
                b = other.b;
                break;
            case 2:
                i = other.i;
                break;
            case 3:
                s = new std::string{*other.s};
                break;
            case 4:
                a = other.a;
                break;
            case 5:
                o = other.o;
            }
            return *this;
        }

        constexpr json_value & operator=(json_value && other) noexcept {
            if(this == &other)
                return *this;
            tag = other.tag;
            switch(tag) {
            case 1:
                b = other.b;
                break;
            case 2:
                i = other.i;
                break;
            case 3:
                s = other.s;
                other.tag = 0;
                break;
            case 4:
                a = std::move(other.a);
                other.tag = 0;
                break;
            case 5:
                o = std::move(other.o);
                other.tag = 0;
            }
            return *this;
        }

        constexpr ~json_value() {
            switch(tag) {
            case 3:
                delete s;
                break;
            case 4:
                a.~vector();
                break;
            case 5:
                o.~vector();
            }
        }

        union {
            bool b;
            int i;
            std::string * s;
            std::vector<json_value> a;
            std::vector<std::pair<std::string, json_value>> o;
        };
        unsigned char tag;
    };
    constexpr parser<json_value> jsonNull{map<[](auto &&) { return json_value{}; }, gat::string<"null">>};
    constexpr parser<json_value> jsonBool{map<[](auto && s) { return json_value{s == "true"}; }, choice<gat::string<"true">, gat::string<"false">>>};
    constexpr parser<json_value> jsonNumber{map<[](auto && v) {
        int i{};
        for(auto c : v)
            i = 10 * i + c - '0';
        return json_value{i};
    }, min<1, gat::chars::digit>>};
    constexpr parser<json_value> jsonString{map<[](auto && v) {
        return json_value{std::string_view{v.data(), v.size()}};
    }, right<gat::chars::exact<'"'>, ahead<min<0, gat::chars::one>, gat::chars::exact<'"'>>>>};
    
    static_assert(!jsonNull(""));
    static_assert(jsonNull("null"));
    static_assert(!jsonBool("test"));
    static_assert(!jsonBool("lol"));
    static_assert(jsonBool("truelol"));
    static_assert(jsonBool("truelol").value.b);
    static_assert(jsonBool("falsexD"));
    static_assert(!jsonBool("falsexD").value.b);
    static_assert(!jsonNumber(""));
    static_assert(jsonNumber("1234"));
    static_assert(jsonNumber("1234").value.i == 1234);
    static_assert(!jsonString(""));
    static_assert(jsonString("\"test\""));
    static_assert(*jsonString("\"test\"").value.s == "test");
}

int main() {
    chars();
    combinators();
    json();
    std::cout << "Hello World!\n";
}
