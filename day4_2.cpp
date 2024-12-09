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

constexpr auto xmas = "MAS"sv;

auto is_valid(int x, int y, int width, int height) {
    return x >= 0 && x < width && y >= 0 && y < height;
}

// Try and find diagonal directions from the current position
// return 1 if both directions spell "MAS"
auto check_all(const std::vector<std::string_view> &lines, int x, int y) {
    // to represent the 4 diagonal directions
    constexpr auto dxys = std::array<dxy, 4>{{{-1, -1}, {-1, 1}, {1, -1}, {1, 1}}};
    constexpr auto directions = std::array{"UL", "DR", "UR", "DL"};

    for (auto i = 0; i < dxys.size(); i++) {
        if (!is_valid(x + dxys[i].dx, y + dxys[i].dy, lines[0].size(), lines.size())) {
            std::println("Invalid: {}, {}", x + dxys[i].dx, y + dxys[i].dy);
            return 0;
        }
    }
    char tl = lines[y + dxys[0].dy][x + dxys[0].dx];
    char tr = lines[y + dxys[1].dy][x + dxys[1].dx];
    char bl = lines[y + dxys[2].dy][x + dxys[2].dx];
    char br = lines[y + dxys[3].dy][x + dxys[3].dx];
    bool first_diagonal = ((tl == 'M' && br == 'S') || (tl == 'S' && br == 'M'));
    bool second_diagonal = ((tr == 'M' && bl == 'S') || (tr == 'S' && bl == 'M'));

    if (first_diagonal && second_diagonal) {
        return 1;
    }
    return 0;
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
            x = lines[y].find('A', x);
            if (x != std::string_view::npos) {
                total += check_all(lines, x, y);
                x++;
            }
        }
    }
    std::println("total: {}", total);
    return 0;
}
