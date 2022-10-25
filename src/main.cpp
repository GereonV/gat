#include <iostream>
#include "chars.hpp"
#include "combinators.hpp"
#include "args.hpp"

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

void args() noexcept {
	using namespace std::literals;
	using namespace gat::args;
	struct result {
		bool enable{};
		bool disable{};
		std::string_view group;
		bool operator==(result const &) const = default;
	};
	constexpr auto parser = parse<result, options<
		option<result, 'e', "enable", &result::enable>,
		option<result, 'd', "disable", &result::disable>
	>{}, options<
		basic_option<result, std::string_view, 'g', "group", &result::group>
	>{}>;
	constexpr char const * cla0[]{ "-" };
	constexpr char const * cla1[]{
		"-edgwheel", "test"
	};
	constexpr char const * cla2[]{
		"-g", "wheel", "-d", "-e", "test"
	};
	constexpr char const * cla3[]{
		"--group=wheel", "-de", "--", "test", "test2"
	};
	constexpr char const * cla4[]{
		"-e", "--", "-d"
	};
	constexpr char const * cla5[]{
		"--e", "--d", "--g=wheel", "-"
	};
	static_assert(parser({cla0}) == std::pair{result{false, false, {}}, std::vector{"-"sv}});
	static_assert(parser({cla1}) == std::pair{result{true, true, "wheel"}, std::vector{"test"sv}});
	static_assert(parser({cla2}) == std::pair{result{true, true, "wheel"}, std::vector{"test"sv}});
	static_assert(parser({cla3}) == std::pair{result{true, true, "wheel"}, std::vector{"test"sv, "test2"sv}});
	static_assert(parser({cla4}) == std::pair{result{true, false, {}}, std::vector{"-d"sv}});
	static_assert(parser({cla5}) == std::pair{result{true, true, "wheel"}, std::vector{"-"sv}});
}

int main() {
    chars();
    combinators();
    args();
    std::cout << "Hello World!\n";
}
