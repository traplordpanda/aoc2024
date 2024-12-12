#include <expected>
#include <fstream>
#include <print>
#include <ranges>
#include <sstream>
#include <string_view>
#include <vector>
#include <cmath>
#include <functional>
#include <queue>
#include <stack>
#include <variant>

using namespace std::string_view_literals;

enum class parse_error {
    invalid_input
};

auto convert_line(std::string_view sv) -> std::expected<std::vector<int>, parse_error> {
    constexpr auto delim = " "sv;
    auto rdigits = std::vector<int>{};
    auto digits = sv | std::views::split(delim);
    int loops = 0;
    
    for (const auto& digit : digits) {
        int num;
        auto digit_sv = std::string_view(digit.data(), digit.size());
        auto [ptr, ec] = std::from_chars(digit_sv.data(), digit_sv.data() + digit_sv.size(), num);
        if (ec == std::errc())
            rdigits.push_back(num);
    }
    return rdigits;
}

using int_functor = std::pair<char, std::function<int(int,int)>>;
using plus = std::plus<int>;
using mult = std::multiplies<int>;
using plus_or_mult = std::variant<plus, mult>;
using op_or_int = std::variant<std::plus<int>, std::multiplies<int>, int>;


auto const ops = std::array<std::function<int(int,int)>, 2> {
    std::plus<int>(),
    std::multiplies<int>()
};


int solve_puzzle() {
    std::ifstream file("day7.txt");
    if (!file) {
        std::println("Error opening input file");
        return 1;
    }

    std::stringstream data;
    data << file.rdbuf();
    file.close();
    auto input = data.str();
    auto lines =
        std::string_view(input) | std::views::split('\n') 
                                | std::views::transform([](auto sv) { return std::string_view(sv.begin(), sv.end()); }) 
                                | std::ranges::to<std::vector>();

    for (auto line : lines) {
        std::println("{}", line);
    }

    for (auto line : lines) {
        auto nums = convert_line(line);
        if (nums.has_value()) {
            if (nums.value().size() > 1) {
                auto first = nums.value().front();
                std::println("first element {}", first);
                auto after_first = std::ranges::subrange(nums.value().begin() + 1, nums.value().end());
                auto combos = pow(2, after_first.size() - 1); 
                std::println("possible combos {}", combos);
                std::print("Numbers: ");
                for (auto num : after_first) {
                    std::print("{} ", num); 
                }
                std::println();
                auto total = 0;
                // masking for lower bit for all combinations 
                for (auto mask = 0; mask < (1 << (after_first.size() - 1)); ++mask) {
                    auto final = 0;
                    std::string char_r;
                    for (auto i = 0; i < (after_first.size()); ++i) {
                        auto lower_bit = (mask << i) & 1;
                        auto op_lower = ops[lower_bit];
                        char_r += std::to_string(first);
                        if (std::is_same_v<std::plus<int>, decltype(op_lower)>) {
                            char_r += " + ";
                        } else {
                            char_r += " * ";
                        }
                        if (i != after_first.size() - 1) {
                    }
                } 
                    std::println("final result {}", char_r);
                }
            }
        std::println();
        }
    }


    return 0;
}
/*
input: 3 + 4 * 3 + 7:w

output:
ops: 

input: 3
output: 3
ops:

input: +
output: 3
ops: + 

input: 4
output: 3 4 
ops: +

input:  *
output: 3 4 
ops: + *

input:  3
output: 3 4 3 
ops: + *

input:  +
output: 3 4 3 + * 
ops: +

input: 7
output: 3 4 3 + * 7
ops: +

input: end
output: 3 4 3 + * 7 +

output: 3 4 3 + * 7 +
stack: 
total: 0

output: + * 7 + 
stack: 3 4 2
total: 0

output: * 7 +
stack: 3
total= 2 + 4 = 6


*/


/*
input: 3 + 4 * 3 + 7
output:
total: 0

lhs: 3
op: plus
rhs: 4
lookahead: mult
output: 3
total: 0

lhs: 4
op: mult
rhs: 3
lookahead: plus
output: 3
total: 12

lhs: 3
op: plus
rhs: 7
lookahead: end
output: 3
total: 22
*/


int main() {
    const auto il = std::array<op_or_int, 7>{3, plus(), 4, mult(), 3, plus(), 7};
    auto input = std::queue<op_or_int>(il.begin(), il.end());
    std::queue<op_or_int> output;
    std::stack<plus_or_mult> ops;
    auto total = 0;
    auto precendence = 0;
    auto lhs = input.front();
    input.pop();
    while (!input.empty()) {
        auto op = input.front();
        input.pop();
        auto rhs = input.front();
        input.pop();
        auto lookahead = input.front();
        using T = std::decay<decltype(op)>;
        using Lookahead = std::decay<decltype(lookahead)>;
        if constexpr (std::is_same_v<T, mult>) {
            total += std::get<mult>(op)(std::get<int>(lhs), std::get<int>(rhs));
        }
        if constexpr (std::is_same_v<T, plus>) {
            if constexpr (std::is_same_v<Lookahead, plus>) {
                total += std::get<int>(lhs) + std::get<int>(rhs);
            } else {
                output.push(lhs);
                output.push(plus());
                output.push(rhs);
            }
        }
    }
}
