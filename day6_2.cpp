#include <expected>
#include <fstream>
#include <print>
#include <ranges>
#include <set>
#include <sstream>
#include <string_view>
#include <vector>
#include <set>

using namespace std::string_view_literals;

enum class parse_error { invalid_input };
enum class direction { up, down, left, right };

struct pos {
    int x;
    int y;
    auto operator<=>(const pos &) const = default;
};

struct posx {
    pos p;
    direction dir;
};

constexpr auto dirchars = std::array{'^', '>', 'v', '<'};

struct guard {
    guard() = default;
    guard(pos p, char dir_char) : p(p) {
        switch (dir_char) {
        case '^':
            dir = direction::up;
            break;
        case '>':
            dir = direction::right;
            break;
        case 'v':
            dir = direction::down;
            break;
        case '<':
            dir = direction::left;
            break;
        }
    }
    pos p;
    direction dir;
    std::set<std::pair<pos, direction>> visited_states;

    auto change_direction() -> void {
        switch (dir) {
        case direction::up:    dir = direction::right; break;
        case direction::right: dir = direction::down;  break;
        case direction::down:  dir = direction::left;  break;
        case direction::left:  dir = direction::up;    break;
        }
    }

    auto move() -> void {
        switch (dir) {
        case direction::up:    p.y--; break;
        case direction::right: p.x++; break;
        case direction::down:  p.y++; break;
        case direction::left:  p.x--; break;
        }
    }
};

struct board {
    std::vector<std::string> lines;
    guard g;
    int width;
    int height;
    pos start_pos;
    char start_dir_char;

    board(std::string_view in_sv) {
        lines = in_sv | std::views::split('\n')
                      | std::views::transform([](auto sv) { return std::string(sv.begin(), sv.end()); })
                      | std::ranges::to<std::vector>();
        width = static_cast<int>(lines[0].size());
        height = static_cast<int>(lines.size());

        // Find guard start
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                char c = lines[y][x];
                if (c == '^' || c == '>' || c == 'v' || c == '<') {
                    g = guard({x,y}, c);
                    start_pos = {x,y};
                    start_dir_char = c;
                    return;
                }
            }
        }
    }

    auto is_valid(int x, int y) const -> bool {
        if (y < 0 || y >= height) return false;
        if (x < 0 || x >= width) return false;
        return true;
    }

    auto is_obstacle(int x, int y) const -> bool {
        if (not(is_valid(x, y))) {
            return false;
        }
        return lines[y][x] == '#';
    }

    auto obstacle_ahead(int x, int y, direction dir) const -> bool {
        switch (dir) {
        case direction::up:    return is_obstacle(x, y-1);
        case direction::right: return is_obstacle(x+1, y);
        case direction::down:  return is_obstacle(x, y+1);
        case direction::left:  return is_obstacle(x-1, y);
        }
        return false;
    }

    auto move_guard() -> void {
        
    }

    auto print_board() {
        for (auto &line : lines) {
            std::println("{}", line);
        }
    }

    auto run_with_obstacle(int x, int y) -> bool {
        auto debug = false;
        if (x == 87 && y == 44) {
            debug = true;
        }
        auto initial_c = lines[y][x];
        lines[y][x] = '#';
        std::set<std::pair<pos, direction>> visited;
        auto initial_pos = g.p;
        auto initial_dir = g.dir;
        while (is_valid(g.p.x, g.p.y)) {
            auto obs_count = 0;
            if (obstacle_ahead(g.p.x, g.p.y, g.dir)) {
                g.change_direction();
            } else {
                visited.insert({g.p, g.dir});
                g.move();
            }
            if (visited.contains({g.p, g.dir})) {
                lines[y][x] = initial_c;
                g.p = initial_pos;
                g.dir = initial_dir;
                return true;
            }
        }

        lines[y][x] = initial_c;
        g.p = initial_pos;
        g.dir = initial_dir;
        return false;
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


    int loop_count = 0;
    for (int y = 0; y < b.height; y++) {
        for (int x = 0; x < b.width; x++) {
            if (x == b.start_pos.x && y == b.start_pos.y) continue;
            if (b.lines[y][x] == '.') {
                if (b.run_with_obstacle(x,y)) {
                    loop_count++;
                }
            }
        }
    }
    std::println("Number of positions that cause a loop: {}", loop_count);
    return 0;
}

