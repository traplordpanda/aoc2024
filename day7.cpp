#include <expected>
#include <algorithm>
#include <fstream>
#include <print>
#include <ranges>
#include <sstream>
#include <string_view>
#include <vector>
#include <numeric>
#include <cmath>
#include <functional>

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

std::array<std::pair<char, std::function<int(int,int)>>, 2> ops = {{
    {'+', std::plus<int>()},
    {'*', std::multiplies<int>()}
}};

int main() {
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
        std::string_view(input) | std::views::split('\n') |
        std::views::transform([](auto sv) { return std::string_view(sv.begin(), sv.end()); }) |
        std::ranges::to<std::vector>();
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

                // masking for lower bit for all combinations 
                for (auto mask = 0; mask < (1 << (after_first.size() - 1)); ++mask) {
                    for (auto i = 0; i < (after_first.size() - 1); ++i) {
                        auto lower_bit = (mask << i) & 1;
                        auto c = std::get<0>(ops[lower_bit]);
                        auto functor = std::get<1>(ops[lower_bit]);
                        auto first = after_first[i];
                        auto second = after_first[i + 1];
                        auto result  = functor(first, second);
                        std::println("{} {} {} = {}", first, c, second, result);

                    } 
                }
                
            }
        std::println();
        }
    }


    return 0;
}

