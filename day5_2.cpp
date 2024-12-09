#include <algorithm>
#include <charconv>
#include <expected>
#include <fstream>
#include <map>
#include <print>
#include <ranges>
#include <set>
#include <sstream>
#include <string_view>
#include <tuple>
#include <vector>

using namespace std::string_view_literals;

enum class parse_error { invalid_input };

auto convert_line(std::string_view sv) -> std::expected<std::tuple<int, int>, parse_error> {
    constexpr auto delim = "|"sv;
    auto rdigits = std::array<int, 2>{};
    auto digits = sv | std::views::split(delim);
    int loops = 0;

    for (const auto &digit : digits) {
        if (loops > 1) {
            return std::unexpected(parse_error::invalid_input);
        }
        auto digit_sv = std::string_view(digit.data(), digit.size());
        auto [ptr, ec] =
            std::from_chars(digit_sv.data(), digit_sv.data() + digit_sv.size(), rdigits[loops++]);
        if (ec != std::errc()) {
            return std::unexpected(parse_error::invalid_input);
        }
        // ensure no trailing characters in string view
        if (ptr != digit_sv.data() + digit_sv.size()) {
            return std::unexpected(parse_error::invalid_input);
        }
    }

    return std::make_tuple(rdigits[0], rdigits[1]);
}

auto check_order(std::map<int, std::set<int>> &order_rules, const std::vector<int> &order) -> bool {
    std::print("numbers: ");
    std::ranges::for_each(order, [](auto num) { std::print("{} ", num); });
    std::println();

    for (auto i = 0; i < order.size(); i++) {
        auto v = std::ranges::views::drop(order, i);
        auto check = std::views::take(v, 1);
        for (auto num : std::ranges::views::drop(v, 1)) {
            if (not(order_rules[check.front()].contains(num))) {
                std::println("{} breaks {}", check.front(), num);
                return false;
            }
        }
    }
    return true;
}

auto fix_order(std::map<int, std::set<int>> &order_rules, std::vector<int> &order) -> void {
    for (auto i = 0; i < order.size(); i++) {
        auto v = std::ranges::views::drop(order, i);
        auto check = std::views::take(v, 1);
        for (auto num : std::ranges::views::drop(v, 1)) {
            if (not(order_rules[check.front()].contains(num))) {
                std::swap(order[i], order[i + 1]);
            }
        }
    }
    if (not(check_order(order_rules, order))) {
        fix_order(order_rules, order);
    }
}

int main() {
    std::ifstream file("day5.txt");
    if (!file) {
        std::println("Error opening input file");
        return 1;
    }

    std::stringstream data;
    data << file.rdbuf();
    file.close();

    auto input = data.str();
    auto input_sv = std::string_view(input);
    auto lines =
        input_sv | std::views::split('\n') |
        std::views::transform([](auto sv) { return std::string_view(sv.begin(), sv.end()); }) |
        std::ranges::to<std::vector>();
    auto total = 0;
    auto order_rules = std::map<int, std::set<int>>{};
    auto order = std::vector<int>(20);

    for (auto line : lines) {
        auto pipe = line.find('|');
        if (pipe != std::string_view::npos) {
            auto result = convert_line(line);
            if (result) {
                auto [first, second] = result.value();
                order_rules[first].insert(second);
            }
        } else {
            auto nums = line | std::views::split(',') | std::views::transform([](auto sv) {
                            return std::string_view(sv.begin(), sv.end());
                        }) |
                        std::views::transform([](auto sv) {
                            int num;
                            std::from_chars(sv.data(), sv.data() + sv.size(), num);
                            return num;
                        }) |
                        std::ranges::to<std::vector>();
            if (not nums.empty()) {
                if (not(check_order(order_rules, nums))) {
                    fix_order(order_rules, nums);
                    std::print("Fixed order: ");
                    std::ranges::for_each(nums, [](auto num) { std::print("{} ", num); });
                    auto middle = nums[nums.size() / 2];
                    std::println("Middle: {}", middle);
                    total += middle;
                    std::println();
                }
            }
        }
    }
    std::println("Total: {}", total);
    return 0;
}
