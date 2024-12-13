// https://en.wikipedia.org/wiki/Reverse_Polish_notation
// https://en.wikipedia.org/wiki/Shunting_yard_algorithm
#include <cassert>
#include <cmath>
#include <expected>
#include <fstream>
#include <functional>
#include <print>
#include <queue>
#include <ranges>
#include <sstream>
#include <stack>
#include <string_view>
#include <variant>
#include <vector>

enum class parse_error { invalid_input, empty_stack, invalid_token };

// setup to read data for the puzzle
using namespace std::string_view_literals;
auto convert_line(std::string_view sv) -> std::expected<std::vector<int>, parse_error> {
    constexpr auto delim = " "sv;
    auto rdigits = std::vector<int>{};
    auto digits = sv | std::views::split(delim);
    int loops = 0;

    for (const auto &digit : digits) {
        int num;
        auto digit_sv = std::string_view(digit.data(), digit.size());
        auto [ptr, ec] = std::from_chars(digit_sv.data(), digit_sv.data() + digit_sv.size(), num);
        if (ec == std::errc())
            rdigits.push_back(num);
    }
    return rdigits;
}

// types for operators to evaluate stacks and create queues
using MultT = std::multiplies<int>;
using PlusT = std::plus<int>;
using plus_or_mult = std::variant<PlusT, MultT>;
using op_or_int = std::variant<PlusT, MultT, int>;

template <> struct std::formatter<PlusT> : std::formatter<std::string_view> {
    auto format(const PlusT &p, std::format_context &ctx) const {
        return std::formatter<std::string_view>::format("+", ctx);
    }
};

template <> struct std::formatter<MultT> : std::formatter<std::string_view> {
    auto format(const MultT &p, std::format_context &ctx) const {
        return std::formatter<std::string_view>::format("*", ctx);
    }
};
auto const ops = std::array<std::function<int(int, int)>, 2>{PlusT(), MultT()};

// line format 3267: 81 40 27
// 3267 is the first number and the rest are the numbers to evaluate
// the first number is the expected result, the rest is the combination of numbers to evaluate
//
auto parse_lines(std::string_view line)
    -> std::expected<std::tuple<int, std::vector<std::vector<op_or_int>>>, parse_error> {
    auto expressions = std::vector<std::vector<op_or_int>>{};
    auto expected_result = 0;
    auto nums = convert_line(line);
    if (nums.has_value()) {
        if (nums.value().size() > 1) {
            auto first = nums.value().front();
            expected_result = first;
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
            // 0 is plus, 1 is mult
            for (auto mask = 0; mask < (1 << (after_first.size() - 1)); ++mask) {
                std::vector<op_or_int> temp;
                for (auto i = 0; i < (after_first.size()); ++i) {
                    temp.push_back(after_first[i]);
                    auto lower_bit = (mask >> i) & 1;
                    if (i != after_first.size() - 1) {
                        if (lower_bit == 0) {
                            temp.push_back(PlusT());
                        } else {
                            temp.push_back(MultT());
                        }
                    }
                }
                expressions.push_back(temp);
            }
        } else {
            return std::unexpected(parse_error::invalid_input);
        }
    }
    return std::tuple(expected_result, expressions);
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

auto evaluate_polish_queue(std::queue<op_or_int> input) -> std::expected<int, parse_error> {
    std::stack<int> eval;
    while (!input.empty()) {
        std::visit(
            [&input, &eval](auto &&op_int) {
                using T = std::decay_t<decltype(op_int)>;
                std::print("{}", op_int);
                if constexpr (std::is_same_v<T, int>) {
                    eval.push(op_int);
                }
                if constexpr (std::is_same_v<T, PlusT>) {
                    auto a = eval.top();
                    eval.pop();
                    auto b = eval.top();
                    eval.pop();
                    auto temp = op_int(a, b);
                    eval.push(temp);
                }
                if constexpr (std::is_same_v<T, MultT>) {
                    auto a = eval.top();
                    eval.pop();
                    auto b = eval.top();
                    eval.pop();
                    auto temp = op_int(a, b);
                    eval.push(temp);
                }
            },
            input.front());
        input.pop();
    }
    std::println();
    if (!eval.empty()) {
        return eval.top();
    }
    return std::unexpected(parse_error::empty_stack);
}

auto evaluate_left_to_right(std::span<op_or_int> in) -> std::expected<int, parse_error> {
    auto total = 0;
    if (in.empty()) {
        return std::unexpected(parse_error::invalid_input);
    }
    total = std::get<int>(in[0]);
    std::println("Input size: {}", in.size());
    for (auto i = 1; i < in.size() - 1; i += 2) {
        if (std::holds_alternative<PlusT>(in[i])) {
            total += std::get<int>(in[i + 1]);
        }
        if (std::holds_alternative<MultT>(in[i])) {
            total *= std::get<int>(in[i + 1]);
        }
        std::println("total: {}", total);
    }
    std::println();
    return total;
}

auto create_polish_queue(std::span<const op_or_int> in)
    -> std::expected<std::queue<op_or_int>, parse_error> {
    auto input = std::queue<op_or_int>(in.begin(), in.end());
    std::queue<op_or_int> output;
    std::stack<plus_or_mult> ops;
    while (!input.empty()) {
        std::visit(
            [&output, &ops](auto &&op_int) {
                using T = std::decay_t<decltype(op_int)>;
                if constexpr (std::is_same_v<T, int>) {
                    output.push(op_int);
                }
                if constexpr (std::is_same_v<T, MultT>) {
                    ops.push(op_int);
                }
                if constexpr (std::is_same_v<T, PlusT>) {
                    while (!ops.empty()) {
                        if (std::holds_alternative<MultT>(ops.top())) {
                            output.push(std::get<MultT>(ops.top()));
                        } else {
                            output.push(std::get<PlusT>(ops.top()));
                        }
                        ops.pop();
                    }
                    ops.push(op_int);
                }
            },
            input.front());
        input.pop();
    }
    // empty ops stack on to output
    while (!ops.empty()) {
        std::visit(
            [&output, &ops](auto &&op_int) {
                using T = std::decay_t<decltype(op_int)>;
                if constexpr (std::is_same_v<T, MultT>) {
                    output.push(op_int);
                }
                if constexpr (std::is_same_v<T, PlusT>) {
                    output.push(op_int);
                }
            },
            ops.top());
        ops.pop();
    }
    return output;
}

auto test() {
    // 81 * 40 + 27
    // 81 + 40 * 27
    auto il = std::array<op_or_int, 5>{81, PlusT(), 40, MultT(), 27};
    std::queue<op_or_int> ltr = {il.begin(), il.end()};
    auto output = create_polish_queue(il);
    auto total = 0;
    auto res = evaluate_left_to_right(il);
    if (res.has_value()) {
        total = res.value();
        std::println("Total: {}", total);
    }
    if (output.has_value()) {
        auto result = evaluate_polish_queue(output.value());
        if (result.has_value()) {
            total = result.value();
            std::println("Total: {}", total);
        }
    } else {
        std::println("Error creating polish queue");
    }
}

auto solve_puzzle() {
    auto result = parse_input("day7.txt");
    auto total = 0;
    if (!result.has_value()) {
        std::println("Error parsing input");
        return -1;
    }
    auto lines = result.value();
    for (auto line : lines) {
        std::println("{}", line);
        auto parse_result = parse_lines(line);
        if (!parse_result.has_value()) {
            continue;
        }
        auto [expected, expressions] = parse_result.value();
        std::println("Expected: {}", expected);
        for (auto exp : expressions) {
            auto output = evaluate_left_to_right(exp);
            if (output.has_value()) {
                std::print("Result: {}  ", output.value());
                if (output.value() == expected) {
                    std::print("Match!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                }
                std::println();
            }
        }
    }
    std::println("Total: {}", total);
    return 0;
}

int main() {
    //test();
    solve_puzzle();
}
