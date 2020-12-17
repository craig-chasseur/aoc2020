#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "util/check.h"
#include "util/io.h"

namespace {

struct Coords {
  int x = 0;
  int y = 0;
  int z = 0;

  template <typename H>
  friend H AbslHashValue(H h, const Coords& coords) {
    return H::combine(std::move(h), coords.x, coords.y, coords.z);
  }

  bool operator==(const Coords& other) const {
    return x == other.x && y == other.y && z == other.z;
  }

  std::vector<Coords> Adjacent() const {
    std::vector<Coords> adjacent;
    adjacent.reserve(27);
    for (int delta_x : {-1, 0, 1}) {
      for (int delta_y : {-1, 0, 1}) {
        for (int delta_z : {-1, 0, 1}) {
          if (delta_x != 0 || delta_y != 0 || delta_z != 0) {
            adjacent.emplace_back(
                Coords{.x = x + delta_x, .y = y + delta_y, .z = z + delta_z});
          }
        }
      }
    }
    return adjacent;
  }
};

class Grid {
 public:
  explicit Grid(const std::vector<std::string>& initial_state) {
    int y = 0;
    for (const std::string& line : initial_state) {
      int x = 0;
      for (const char c : line) {
        if (c == '#') {
          active_.emplace(Coords{.x = x, .y = y, .z = 0});
        }
        ++x;
      }
      ++y;
    }
  }

  void Step() {
    absl::flat_hash_set<Coords> next_active_;
    for (const Coords& active_cube : active_) {
      if (NextActive(active_cube)) {
        next_active_.insert(active_cube);
      }
      for (const Coords& adjacent : active_cube.Adjacent()) {
        if (NextActive(adjacent)) {
          next_active_.insert(adjacent);
        }
      }
    }
    active_ = std::move(next_active_);
  }

  int NumActive() const {
    return active_.size();
  }

 private:
  int CountAdjacent(const Coords& coords) const {
    int adjacent = 0;
    for (const Coords& adjacent_cell : coords.Adjacent()) {
      adjacent += active_.contains(adjacent_cell);
    }
    return adjacent;
  }

  bool NextActive(const Coords& coords) const {
    const int neighbors_active = CountAdjacent(coords);
    if (active_.contains(coords)) {
      return neighbors_active == 2 || neighbors_active == 3;
    }
    return neighbors_active == 3;
  }

  absl::flat_hash_set<Coords> active_;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK(2 == argc);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);

  Grid grid(lines);
  for (int i = 0; i < 6; ++i) {
    grid.Step();
  }
  std::cout << grid.NumActive() << "\n";

  return 0;
}
