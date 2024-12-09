#include <expected>
#include <fstream>
#include <print>
#include <ranges>
#include <sstream>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

struct dxy {
    int dx;
    int dy;
};

auto is_valid(int x, int y, int width, int height) {
    return x >= 0 && x < width && y >= 0 && y < height;
}

auto check_all(const std::vector<std::string_view> &lines, int x, int y) {
    auto total = 0;
    // to represent the 8 directions
    constexpr auto dxys =
        std::array<dxy, 8>{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {1, -1}, {-1, 1}, {1, 1}}};
    
    constexpr auto xmas = "XMAS"sv;
    for (auto i = 0; i < dxys.size(); i++) {
        auto curr_x = x;
        auto curr_y = y;
        bool valid = true;
        
        for (auto k = 0; k < xmas.size(); k++) {
            if (!is_valid(curr_x, curr_y, lines[0].size(), lines.size())) {
                valid = false;
                break;
            }
            if (lines[curr_y][curr_x] != xmas[k]) {
                valid = false;
                break;
            }
            curr_x += dxys[i].dx;
            curr_y += dxys[i].dy;
        }
        
        if (valid) {
            total++;
        }
    }
    return total;
}

int main() {
    std::ifstream file("day4.txt");
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
    auto lines =
        input_sv | std::views::split('\n') |
        std::views::transform([](auto sv) { return std::string_view(sv.begin(), sv.end()); }) |
        std::ranges::to<std::vector>();

    // Remove any empty lines at the end
    while (!lines.empty() && lines.back().empty()) {
        lines.pop_back();
    }

    for (int y = 0; y < lines.size(); y++) {
        auto x = 0;
        while (x != std::string_view::npos && x < lines[y].size()) {
            x = lines[y].find('X', x);
            if (x != std::string_view::npos) {
                total += check_all(lines, x, y);
                x++;
            }
        }
    }
    std::println("total: {}", total);
    return 0;
}

