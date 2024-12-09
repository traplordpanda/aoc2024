#include <array>
#include <expected>
#include <fstream>
#include <print>
#include <ranges>
#include <sstream>
#include <string_view>

using namespace std::string_view_literals;

enum class parse_error { invalid_input };

struct day3tokens {
    auto constexpr static start = "mul("sv;
    auto constexpr static close = ")"sv;
    auto constexpr static delim = ","sv;
    auto constexpr static enable = "do()"sv;
    auto constexpr static disable = "don't()"sv;
};

auto convert_line(std::string_view sv) -> std::expected<std::tuple<int, int>, parse_error> {
    auto rdigits = std::array<int, 2>{};
    auto digits = sv | std::views::split(day3tokens::delim);
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

int main() {
    std::ifstream file("day3.txt");
    if (!file) {
        std::println("Error opening input file");
        return 1;
    }

    std::stringstream data;
    data << file.rdbuf();
    file.close();

    auto input = data.str();
    auto input_sv = std::string_view(input);
    auto total = 0;
    auto current = size_t{0};
    auto enabled = true;
    while (true) {
        auto mult_pos = input_sv.find(day3tokens::start, current);
        if (mult_pos == std::string::npos)
            break;

        auto search_string = input_sv.substr(current, mult_pos - current);
        auto stop = search_string.find(day3tokens::disable);
        if (stop != std::string::npos) {
            // find the location of the stop token in original sv
            auto stop_pos = input_sv.find(day3tokens::disable, current);
            // set new current where 'do()' is found
            current = input_sv.find(day3tokens::enable, stop_pos);
            mult_pos = input_sv.find(day3tokens::start, current);

            if (current == std::string::npos || mult_pos == std::string::npos) {
                break;
            }
        }

        auto mult_start = mult_pos + day3tokens::start.size();
        auto next_paren = input_sv.find(day3tokens::close, mult_pos);
        auto mult_str = input_sv.substr(mult_pos, next_paren - mult_pos + 1);
        auto digits = input_sv.substr(mult_start, next_paren - mult_start);
        if (auto result = convert_line(digits); result.has_value()) {
            auto [a, b] = result.value();
            total = total + a * b;
            current = next_paren;
            std::println("successful parse {} = {} * {} ", mult_str, a, b);
        } else {
            current = mult_start;
            std::println("parsing failure at {}", mult_str);
        }
        std::println();
    }
    std::println("total: {}", total);
    return 0;
}
