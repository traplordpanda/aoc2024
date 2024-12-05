#include <vector>
#include <print>
#include <tuple>
#include <fstream>
#include <string>
#include <charconv>
#include <string_view>
#include <ranges>

auto convert_line(std::string_view sv, std::vector<int>& list) {
    using std::operator""sv;
    constexpr auto delim = " "sv;
    for (const auto digit : std::views::split(sv, delim)) {
        int value;
        if (auto [ptr, ec] = std::from_chars(digit.data(), digit.data() + digit.size(), value); ec == std::errc()) {
            list.push_back(value);
        }
    }
}

auto check_sequence(const std::vector<int>& list, std::size_t skip = std::numeric_limits<size_t>::max()) -> std::tuple<bool, std::size_t> {
    if (list.size() <= 1) return {true, 0};
    
    auto increasing = list[0] < list[1];
    
    for (int i = 0; i < list.size() - 1; i++) {
        if (i == skip) continue;
        if (list[i] > list[i + 1] && increasing) {
            return {false, i};
        }
        if (list[i] < list[i + 1] && !increasing) {
            return {false, i};
        }
        if (not(std::abs(list[i] - list[i + 1]) > 0 && std::abs(list[i] - list[i + 1]) < 4)) {
            return {false, i};
        }
    }
    return {true, skip};
}

auto safe_list(const std::vector<int>& list) -> bool {
    auto [result, skip] = check_sequence(list);
    if (!result) {
        auto [result, _] = check_sequence(list, skip);
        return result;
    }
    return result;
}


int main() {
    std::string line;
    auto total_safe = 0;
    auto file = std::ifstream("day2.txt");

    std::vector<int> list1;
    while(std::getline(file, line)) {
        convert_line(line, list1);
        auto result = safe_list(list1);
        if (result) {
            total_safe++;
        }
        list1.clear();
    }
    std::println("Total safe lines: {}", total_safe);
    return 0;
}

