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
        width = (int)lines[0].size();
        height = (int)lines.size();

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

    auto is_obstacle(int x, int y) const -> bool {
        if (y < 0 || y >= height) return false;
        if (x < 0 || x >= width) return false;
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

    // Run the guard simulation on the current board state
    // Return true if a loop is detected
    auto run_simulation(pos start_p, char start_c) const -> bool {
        guard local_guard(start_p, start_c);

        // Track visited states: (pos, direction)
        std::set<std::pair<pos, direction>> visited;
        visited.insert({local_guard.p, local_guard.dir});

        while (local_guard.p.x >= 0 && local_guard.p.x < width &&
               local_guard.p.y >= 0 && local_guard.p.y < height) {
            // If obstacle ahead, turn right
            if (obstacle_ahead(local_guard.p.x, local_guard.p.y, local_guard.dir)) {
                local_guard.change_direction();
            } else {
                // Move forward
                local_guard.move();
                // Check if this state is already visited
                if (visited.contains({local_guard.p, local_guard.dir})) {
                    // Loop detected
                    return true;
                }
                visited.insert({local_guard.p, local_guard.dir});
            }
        }
        return false; // Guard left the map, no loop
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
    auto original_board = board(std::string_view(input));

    int loop_count = 0;

    // We'll try placing an obstacle in every '.' cell except the guard's start
    for (int y = 0; y < original_board.height; y++) {
        for (int x = 0; x < original_board.width; x++) {
            // Can't place at guard start
            if (x == original_board.start_pos.x && y == original_board.start_pos.y) continue;

            // Only place obstacle in '.' spots
            if (original_board.lines[y][x] == '.') {
                // Copy board
                board test_board = original_board;
                // Place new obstacle
                test_board.lines[y][x] = '#';

                // Run simulation on modified board
                bool loop = test_board.run_simulation(original_board.start_pos, original_board.start_dir_char);
                if (loop) {
                    loop_count++;
                }
            }
        }
    }

    std::println("Number of positions that cause a loop: {}", loop_count);
    return 0;
}

