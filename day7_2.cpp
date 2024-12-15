// https://en.wikipedia.org/wiki/Reverse_Polish_notation
// https://en.wikipedia.org/wiki/Shunting_yard_algorithm
#include <cassert>
#include <cmath>
#include <expected>
#include <fstream>
#include <print>
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

auto concat_int64(std::int64_t first, std::int64_t second) -> std::int64_t {
    auto size = 1;
    auto temp = second;
    while (temp > 0) {
        temp /= 10;
        size = size * 10;
    }
    return size * first + second;
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
        auto combos = pow(3, after_first.size() - 1);
        for (auto mask = 0; mask < combos; ++mask) {
            std::int64_t temp = after_first[0];
            for (int i = 1; i < after_first.size(); i++) {
                auto switcher = (mask / static_cast<int>(std::pow(3, i - 1))) % 3;
                if (switcher == 0) {
                    temp += after_first[i];
                } else if (switcher == 1) {
                    temp *= after_first[i];
                } else if (switcher == 2) {
                    temp = concat_int64(temp, after_first[i]);
                }
            }
            if (temp == check) {
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
            auto result = parse_lines(line);
            if (result.has_value()) {
                total += result.value();
            }
        }
    }
    std::println("line size = {}", lines.value().size());
    std::println("Total {}", total);
}
