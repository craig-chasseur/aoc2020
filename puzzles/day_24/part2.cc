#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/strings/string_view.h"
#include "util/check.h"
#include "util/io.h"

namespace {

class HexCoords {
 public:
  HexCoords() = default;

  HexCoords East() const {
    HexCoords next(*this);
    ++next.x_;
    return next;
  }

  HexCoords West() const {
    HexCoords next(*this);
    --next.x_;
    return next;
  }

  HexCoords SouthEast() const {
    HexCoords next(*this);
    --next.y_;
    return next;
  }

  HexCoords SouthWest() const {
    HexCoords next(*this);
    --next.x_;
    --next.y_;
    return next;
  }

  HexCoords NorthEast() const {
    HexCoords next(*this);
    ++next.x_;
    ++next.y_;
    return next;
  }

  HexCoords NorthWest() const {
    HexCoords next(*this);
    ++next.y_;
    return next;
  }

  HexCoords ApplyPath(absl::string_view path) const {
    HexCoords current(*this);
    while (!path.empty()) {
      const char c = path.front();
      path.remove_prefix(1);
      switch (c) {
        case 'e':
          current = current.East();
          break;
        case 'w':
          current = current.West();
          break;
        case 'n': {
          CHECK(!path.empty());
          const char next_c = path.front();
          path.remove_prefix(1);
          switch (next_c) {
            case 'e':
              current = current.NorthEast();
              break;
            case 'w':
              current = current.NorthWest();
              break;
            default:
              CHECK_FAIL();
          }
          break;
        }
        case 's': {
          CHECK(!path.empty());
          const char next_c = path.front();
          path.remove_prefix(1);
          switch (next_c) {
            case 'e':
              current = current.SouthEast();
              break;
            case 'w':
              current = current.SouthWest();
              break;
            default:
              CHECK_FAIL();
          }
          break;
        }
        default:
          CHECK_FAIL();
      }
    }
    return current;
  }

  std::vector<HexCoords> AdjacentHexes() const {
    return {East(), West(), SouthEast(), SouthWest(), NorthEast(), NorthWest()};
  }

  bool operator==(const HexCoords& other) const {
    return x_ == other.x_ && y_ == other.y_;
  }

  template <typename H>
  friend H AbslHashValue(H h, const HexCoords& hex_coords) {
    return H::combine(std::move(h), hex_coords.x_, hex_coords.y_);
  }

 private:
  int x_ = 0;
  int y_ = 0;
};

class Floor {
 public:
  explicit Floor(const std::vector<std::string>& initial_tile_paths) {
    for (absl::string_view line : initial_tile_paths) {
      HexCoords tile = HexCoords().ApplyPath(line);
      auto [iter, inserted] = black_tiles_.emplace(tile);
      if (!inserted) black_tiles_.erase(iter);
    }
  }

  void Step() {
    absl::flat_hash_set<HexCoords> candidate_tiles = black_tiles_;
    for (const HexCoords black_tile : black_tiles_) {
      std::vector<HexCoords> adjacent = black_tile.AdjacentHexes();
      candidate_tiles.insert(adjacent.begin(), adjacent.end());
    }

    absl::flat_hash_set<HexCoords> next_black;
    for (const HexCoords candidate_tile : candidate_tiles) {
      if (BlackInNextIteration(candidate_tile)) {
        next_black.insert(candidate_tile);
      }
    }

    black_tiles_ = std::move(next_black);
  }

  int BlackTiles() const {
    return black_tiles_.size();
  }

 private:
  int CountAdjacentBlack(HexCoords hex) const {
    int adjacent_black = 0;
    for (const HexCoords adjacent_hex : hex.AdjacentHexes()) {
      adjacent_black += black_tiles_.contains(adjacent_hex);
    }
    return adjacent_black;
  }

  bool BlackInNextIteration(HexCoords hex) const {
    const int adjacent_black = CountAdjacentBlack(hex);
    if (black_tiles_.contains(hex)) {
      if (adjacent_black == 0 || adjacent_black > 2) return false;
      return true;
    }
    return adjacent_black == 2;
  }

  absl::flat_hash_set<HexCoords> black_tiles_;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK(2 == argc);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);

  Floor floor(lines);
  for (int i = 0; i < 100; ++i) {
    floor.Step();
  }
  std::cout << floor.BlackTiles() << "\n";

  return 0;
}
