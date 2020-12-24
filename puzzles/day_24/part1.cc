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

}  // namespace

int main(int argc, char** argv) {
  CHECK(2 == argc);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);

  absl::flat_hash_set<HexCoords> black_tiles;
  for (absl::string_view line : lines) {
    HexCoords tile = HexCoords().ApplyPath(line);
    auto [iter, inserted] = black_tiles.emplace(tile);
    if (!inserted) black_tiles.erase(iter);
  }
  std::cout << black_tiles.size() << "\n";

  return 0;
}
