#include <print>
#include <string_view>
#include <ranges>
#include <fstream>
#include <expected>
#include <array>
#include <sstream>

using namespace std::string_view_literals;

enum class parse_error {
    invalid_input
};

auto convert_line(std::string_view sv) -> std::expected<std::tuple<int, int>, parse_error> {
    constexpr auto delim = ","sv;
    auto rdigits = std::array<int, 2>{};
    auto digits = sv | std::views::split(delim);
    int loops = 0;
    
    for (const auto& digit : digits) {
        if (loops > 1) {
            return std::unexpected(parse_error::invalid_input);
        }
        auto digit_sv = std::string_view(digit.data(), digit.size());
        auto [ptr, ec] = std::from_chars(digit_sv.data(), digit_sv.data() + digit_sv.size(), rdigits[loops++]);
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
    constexpr auto mult_delim = "mul("sv;
    constexpr auto mult_close = ")"sv;
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
    auto current_pos = size_t{0};
    
    while (true) {
        auto mult_pos = input_sv.find(mult_delim, current_pos);
        if (mult_pos == std::string::npos) break;
        
        auto mult_start = mult_pos + mult_delim.size();
        auto next_paren = input_sv.find(mult_close, mult_pos);
        auto mult_str = input_sv.substr(mult_pos, next_paren - mult_pos + 1);
        auto digits = input_sv.substr(mult_start, next_paren - mult_start);
        if (auto result = convert_line(digits); result.has_value()) {
            auto [a, b] = result.value();
            total = total + a * b;
            current_pos = next_paren;
            std::println("successful parse {} = {} * {} ", mult_str, a, b);
        }
        else {
            current_pos = mult_start;
        }
    }
    std::println("total: {}", total);
    return 0;
}
