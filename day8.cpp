#include <fstream>
#include <map>
#include <print>
#include <ranges>
#include <set>
#include <string>
#include <vector>

using namespace std::string_view_literals;
struct Point {
    int x;
    int y;
    auto operator<=>(const Point &p) const = default;
};

auto calc_antinode(const Point &p1, const Point &p2) -> Point {
    auto dy = p1.y + p1.y - p2.y;
    auto dx = p1.x + p1.x - p2.x;
    return {dx, dy};
}

struct Board {
  public:
    Board(const std::string &path) : board(), points() {
        std::ifstream file(path);
        constexpr auto delim = "\n"sv;
        board = std::ranges::istream_view<std::string>(file) |
                std::ranges::to<std::vector<std::string>>();
        for (int y = 0; y < board.size(); y++) {
            for (int x = 0; x < board[0].size(); x++) {
                if (board[y][x] != '.') {
                    points[board[y][x]].push_back({x, y});
                }
            }
        }
    }

    auto print_board() {
        std::println(" ");
        for (int i = 0; i < board[0].size(); i++) {
            std::print("{}", i % 10);
        }
        for (int i = 0; i < board.size(); i++) {
            std::println("{} {}", i % 10, board[i]);
        }
    }

    auto is_valid(Point p) {
        return p.x >= 0 && p.x < board[0].size() && p.y >= 0 && p.y < board.size();
    }

    auto print_points() {
        for (const auto &[key, value] : points) {
            std::println("{}", key);
            for (const auto &point : value) {
                std::println("({}, {})", point.x, point.y);
            }
        }
    }

    auto write_node(const char c, const Point p) -> void {
        if (is_valid(p)) {
            board[p.y][p.x] = c;
        }
        return;
    }

    auto get_node(const Point p) -> char {
        if (is_valid(p)) {
            return board[p.y][p.x];
        }
        return '.';
    }

    auto write_antinodes() -> void {
        int total = 0;
        for (const auto &[key, value] : points) {
            if (value.size() < 2) {
                continue;
            }
            for (int i = 0; i < value.size(); i++) {
                for (int k = i + 1; k < value.size() + i; k++) {
                    auto idx = k % value.size();
                    auto p = value[i];
                    auto next_p = value[idx];
                    auto antinode = calc_antinode(p, next_p);
                    if (is_valid(antinode)) {
                        antinodes.insert(antinode);
                    }
                    if (get_node(antinode) == '.') {
                        write_node('#', antinode);
                    }
                }
            }
        }
    }

    auto get_board() const -> std::vector<std::string> { return board; }

    auto antinode_count() const -> int { return antinodes.size(); }

  private:
    std::vector<std::string> board;
    std::map<char, std::vector<Point>> points;
    std::set<Point> antinodes;
};

int main() {
    constexpr auto path = "day8.txt";
    auto board = Board(path);
    board.print_board();
    board.print_points();
    board.write_antinodes();
    board.print_board();
    auto totalcrunchbangs = 0;
    for (const auto &row : board.get_board()) {
        for (const auto &c : row) {
            if (c == '#') {
                totalcrunchbangs++;
            }
        }
    }
    std::println("Total crunchbangs: {}", totalcrunchbangs);
    std::println("Total antinode count: {}", board.antinode_count());
}
