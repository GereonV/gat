#ifndef GAT_ARGS_HPP
#define GAT_ARGS_HPP

// See https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html

#include <span>
#include <stdexcept>
#include "chars.hpp"
#include "combinators.hpp"

namespace gat::args {

	template<typename ReturnT, auto options, auto argoptions, std::size_t Extent> // = std::dynamic_extent>
	constexpr std::pair<ReturnT, std::vector<std::string_view>> parse_arguments(std::span<char const * const, Extent> input) {
		ReturnT res;
		std::vector<std::string_view> args;
		for(auto it = input.begin(), end = input.end(); it != end; ++it) {
			std::string_view sv{*it};
			if(sv.size() == 1 || sv[0] != '-') {
				args.emplace_back(sv);
				continue;
			}
			if(sv[1] == '-') {
				sv.remove_prefix(2);
				if(sv.empty()) {
					while(++it != end)
						args.emplace_back(*it);
					break;
				}
				auto eq = sv.find('=');
				if(eq == sv.npos) {
					if(auto opt = options(sv); opt) {
						res.*opt = true;
						continue;
					}
				} else {
					auto value = sv.substr(eq + 1);
					sv = sv.substr(0, eq);
					if(auto opt = argoptions(sv); opt) {
						res.*opt = value;
						continue;
					}
				}
				throw std::invalid_argument{std::string{"Unknown long option: \""}.append(sv) + '"'};
			}
			for(std::size_t i{1}; i < sv.size(); ++i) {
				auto c = sv[i];
				if(auto opt = options(c); opt) {
					res.*opt = true;
					continue;
				}
				auto opt = argoptions(c);
				if(!opt)
					throw std::invalid_argument{std::string{"Unknown option: '"} + c + '\''};
				res.*opt = sv.size() == ++i ? *++it : sv.substr(i);
				break;
			}
		}
		return {std::move(res), std::move(args)};
	}

}

#endif // GAT_ARGS_HPP
