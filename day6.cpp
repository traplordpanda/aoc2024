#include <expected>
#include <fstream>
#include <print>
#include <ranges>
#include <set>
#include <sstream>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

enum class parse_error { invalid_input };

enum class direction { up, down, left, right };

struct pos {
    int x;
    int y;
    auto operator<=>(const pos &) const = default;
};

constexpr auto dirchars = std::array{'^', '>', 'v', '<'};

struct guard {
    guard() = default;
    guard(pos p, char dir) : p(p), visited{p} {
        switch (dir) {
        case '^':
            this->dir = direction::up;
            break;
        case '>':
            this->dir = direction::right;
            break;
        case 'v':
            this->dir = direction::down;
            break;
        case '<':
            this->dir = direction::left;
            break;
        }
    }

    pos p;
    direction dir;
    std::set<pos> visited;
    auto change_direction() -> void {
        switch (dir) {
        case direction::up:
            dir = direction::right;
            break;
        case direction::right:
            dir = direction::down;
            break;
        case direction::down:
            dir = direction::left;
            break;
        case direction::left:
            dir = direction::up;
            break;
        }
    }
    auto move() -> void {
        switch (dir) {
        case direction::up:
            p.y--;
            this->visited.insert(p);
            break;
        case direction::right:
            p.x++;
            this->visited.insert(p);
            break;
        case direction::down:
            p.y++;
            this->visited.insert(p);
            break;
        case direction::left:
            p.x--;
            this->visited.insert(p);
            break;
        }
    }
};

struct obstacle {
    int x;
    int y;
};

auto find_position(std::string_view sv, int y) -> std::expected<pos, parse_error> {
    for (auto ch : dirchars) {
        if (auto x = sv.find(ch); x != std::string::npos) {
            return pos{x, y};
        }
    }
    return std::unexpected(parse_error::invalid_input);
}

struct board {
    board(std::string_view in_sv) {
        this->lines =
            in_sv | std::views::split('\n') |
            std::views::transform([](auto sv) { return std::string(sv.begin(), sv.end()); }) |
            std::ranges::to<std::vector>();

        for (auto y = 0; y < lines.size(); y++) {
            if (auto res = find_position(lines[y], y); res.has_value()) {
                g = guard(res.value(), lines[y][res.value().x]);
                break;
            }
        }
    }
    std::vector<std::string> lines;
    guard g;
    auto print_board() {
        for (auto v : g.visited) {
            lines[v.y][v.x] = 'X';
        }
        for (auto line : lines) {
            std::println("{}", line);
        }
    }

    auto is_obstacle(int x, int y) -> bool {
        if (y < 0 || y >= lines.size()) {
            return false;
        }
        if (x < 0 || x >= lines[y].size()) {
            return false;
        }
        return lines[y][x] == '#';
    }

    auto is_obstacle(int x, int y, direction dir) -> bool {
        switch (dir) {
        case direction::up:
            return is_obstacle(x, y - 1);
        case direction::right:
            return is_obstacle(x + 1, y);
        case direction::down:
            return is_obstacle(x, y + 1);
        case direction::left:
            return is_obstacle(x - 1, y);
        default:
            return false;
        }
    }

    auto is_obstacle() -> bool { return is_obstacle(g.p.x, g.p.y, g.dir); }

    auto guard_position() -> std::expected<pos, parse_error> {
        for (int y = 0; y < lines.size(); y++) {
            for (int x = 0; x < lines[y].size(); x++) {
                if (lines[y][x] == '^') {
                    return pos{x, y};
                }
            }
        }
        return std::unexpected(parse_error::invalid_input);
    }

    auto move_guard() -> void {
        auto width = this->lines[0].size();
        auto height = this->lines.size();
        while (this->g.p.x < width && this->g.p.x >= 0 && this->g.p.y < height &&
               this->g.p.y >= 0) {
            pos new_p = g.p;

            switch (this->g.dir) {
            case direction::up:
                new_p.y--;
                break;
            case direction::down:
                new_p.y++;
                break;
            case direction::left:
                new_p.x--;
                break;
            case direction::right:
                new_p.x++;
                break;
            }

            if (new_p.x >= 0 && new_p.x < width && new_p.y >= 0 && new_p.y < height) {
                if (is_obstacle(new_p.x, new_p.y)) {
                    g.change_direction();
                } else {
                    g.move();
                }
            } else {
                break;
            }
        }
    }
};

int main() {
    std::ifstream file("day6.txt");
    if (!file) {
        std::println("Error opening input file");
        return 1;
    }

    std::stringstream data;
    data << file.rdbuf();
    file.close();

    auto input = data.str();
    auto b = board(std::string_view(input));
    b.move_guard();
    b.print_board();
    std::println("total visited: {}", b.g.visited.size());
    return 0;
}
