// https://en.wikipedia.org/wiki/Reverse_Polish_notation
// https://en.wikipedia.org/wiki/Shunting_yard_algorithm
#include <cassert>
#include <cmath>
#include <expected>
#include <fstream>
#include <numeric>
#include <print>
#include <queue>
#include <ranges>
#include <sstream>
#include <string_view>
#include <vector>

enum class parse_error { invalid_input, empty_stack, invalid_token };

// setup to read data for the puzzle
using namespace std::string_view_literals;
auto convert_line(std::string_view sv) -> std::vector<std::int64_t> {
    constexpr auto delim = " "sv;
    auto rdigits = std::vector<std::int64_t>{};
    auto digits = sv | std::views::split(delim);
    int loops = 0;

    for (const auto &digit : digits) {
        std::int64_t num;
        auto digit_sv = std::string_view(digit.data(), digit.size());
        auto [ptr, ec] = std::from_chars(digit_sv.data(), digit_sv.data() + digit_sv.size(), num);
        if (ec == std::errc())
            rdigits.push_back(num);
    }
    return rdigits;
}

// line format 3267: 81 40 27
// 3267 is the first number and the rest are the numbers to evaluate
// the first number is the expected result, the rest is the combination of numbers to evaluate
auto parse_lines(std::string_view line) -> std::expected<std::int64_t, parse_error> {
    auto nums = convert_line(line);
    if (nums.size() < 2) {
        return std::unexpected(parse_error::invalid_input);
    }
    auto total = 0;
    auto expected_result = 0;
    auto check = nums[0];
    auto after_first = std::ranges::subrange(nums.begin() + 1, nums.end());
    if (nums.size() > 1) {
        auto combos = (1 << (after_first.size() - 1));
        std::println("Combos {}", combos);
        for (auto mask = 0; mask < (1 << (after_first.size() - 1)); ++mask) {
            std::int64_t temp = after_first[0];
            for (int i = 1; i < after_first.size(); i++) {
                auto use_mult = ((mask >> (i - 1)) & 1);
                // std::print("{} ", temp);
                if (!use_mult) {
                    temp += after_first[i];
                    std::print("+ {};", after_first[i]);
                } else {
                    temp *= after_first[i];
                    std::print("* {};", after_first[i]);
                }
            }
            std::println(" = {} ", temp);
            if (temp == check) {
                std::println("found match");
                return check;
            }
        }
    }
    return std::unexpected(parse_error::invalid_input);
}

auto parse_input(const std::string &path) -> std::expected<std::vector<std::string>, parse_error> {

    std::ifstream file(path);
    if (!file) {
        return std::unexpected(parse_error::invalid_input);
    }
    std::stringstream data;
    data << file.rdbuf();
    file.close();
    auto input = data.str();
    auto lines = std::string_view(input) | std::views::split('\n') |
                 std::views::transform([](auto sv) { return std::string(sv.begin(), sv.end()); }) |
                 std::ranges::to<std::vector>();
    return lines;
}

// end setup for puzzle

int main() {
    auto lines = parse_input("day7.txt");
    std::int64_t total = 0;

    if (lines.has_value()) {
        for (auto &line : lines.value()) {
            std::println("{}", line);
            auto result = parse_lines(line);
            if (result.has_value()) {
                std::println("Result {}", result.value());
                total += result.value();
            }
        }
    }
    std::println("line size = {}", lines.value().size());
    std::println("Total {}", total);
}
