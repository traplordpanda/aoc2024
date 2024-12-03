#include <vector>
#include <print>
#include <fstream>
#include <iostream>
#include <string>
#include <charconv>
#include <string_view>

auto convert_line(std::string_view sv, std::vector<int>& list) {
    std::size_t pos = 0;
    while (pos < sv.length()) {
        while (pos < sv.length() && !std::isdigit(sv[pos])) {
            pos++;
        }
        auto numstart = pos;
        while (pos < sv.length() && std::isdigit(sv[pos])) {
            pos++;
        }

        int value;
        if (numstart < pos) {
            if (auto [ptr, ec] = std::from_chars(sv.data() + numstart, sv.data() + pos, value); ec == std::errc()) {
                list.push_back(value);
            }
        }
    }        
}

auto safe_list(std::vector<int>& list) -> bool {
    auto increasing = bool{};
    if (list.size() > 1) {
        increasing = list[0] < list[1];
    }

    for (int i = 0; i < list.size() - 1; i++) {
        if (list[i] > list[i + 1] && increasing) {
            return false;
        }
        if (list[i] < list[i + 1] && !increasing) {
            return false; 
        }
        if (not(std::abs(list[i] - list[i + 1]) > 0 && std::abs(list[i] - list[i + 1]) < 4)) {
            return false;
        }
    } 
    return true;
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

