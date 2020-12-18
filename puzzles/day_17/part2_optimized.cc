#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "util/check.h"
#include "util/io.h"

namespace {

struct Coords {
  int x = 0;
  int y = 0;
  int z = 0;
  int w = 0;

  template <typename H>
  friend H AbslHashValue(H h, const Coords& coords) {
    return H::combine(std::move(h), coords.x, coords.y, coords.z, coords.w);
  }

  bool operator==(const Coords& other) const {
    return x == other.x && y == other.y && z == other.z && w == other.w;
  }

  std::vector<Coords> Adjacent() const {
    std::vector<Coords> adjacent;
    adjacent.reserve(80);
    for (int delta_x : {-1, 0, 1}) {
      for (int delta_y : {-1, 0, 1}) {
        for (int delta_z : {-1, 0, 1}) {
          for (int delta_w : {-1, 0, 1}) {
            if (delta_x != 0 || delta_y != 0 || delta_z != 0 || delta_w != 0) {
              adjacent.emplace_back(Coords{.x = x + delta_x,
                                           .y = y + delta_y,
                                           .z = z + delta_z,
                                           .w = w + delta_w});
            }
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
          active_.emplace(Coords{.x = x, .y = y, .z = 0, .w = 0});
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

  int NumActive() const { return active_.size(); }

 private:
  int CountAdjacent(const Coords& coords) const {
    int adjacent = 0;
    for (const Coords& adjacent_cell : coords.Adjacent()) {
      if (active_.contains(adjacent_cell)) {
        if (++adjacent == 4) return 4;
      }
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

  const absl::Time pre = absl::Now();
  for (int i = 0; i < 10; ++i) {
    Grid grid(lines);
    for (int i = 0; i < 6; ++i) {
      grid.Step();
    }
    CHECK(1620 == grid.NumActive());
  }
  const absl::Time post = absl::Now();
  std::cout << (post - pre) << "\n";

  return 0;
}
