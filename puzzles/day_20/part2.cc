#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "puzzles/day_20/bit_square.h"
#include "util/check.h"
#include "util/io.h"

namespace {

class Tile {
 public:
  explicit Tile(const std::vector<std::string>& line_group) : rep_(10) {
    CHECK(line_group.size() == 11);
    CHECK(1 == std::sscanf(line_group.front().c_str(), "Tile %d:", &id_));
    for (int row_idx = 0; row_idx < 10; ++row_idx) {
      for (int col_idx = 0; col_idx < 10; ++col_idx) {
        rep_.Set(row_idx, col_idx, line_group[row_idx + 1][col_idx] == '#');
      }
    }
  }

  int id() const { return id_; }

  std::vector<std::uint16_t> GetEdgeStrips() const {
    return {rep_.SliceHorizontal(0),
            rep_.SliceHorizontal(9),
            rep_.SliceVertical(0),
            rep_.SliceVertical(9),
            rep_.FlipHorizonal().SliceHorizontal(0),
            rep_.FlipHorizonal().SliceHorizontal(9),
            rep_.FlipVertical().SliceVertical(0),
            rep_.FlipVertical().SliceVertical(9)};
  }

 private:
  int id_ = 0;
  BitSquare rep_;
};

std::vector<std::vector<Tile>> SolvePuzzle(std::vector<Tile> tiles) {
  const int dim = std::sqrt(tiles.size());
  CHECK(dim * dim == tiles.size());

  absl::flat_hash_map<std::uint16_t, int> edge_counts;
  for (const Tile& tile : tiles) {
    for (std::uint16_t edge : tile.GetEdgeStrips()) {
      ++edge_counts[edge];
    }
  }

  std::vector<Tile> corners;
  std::vector<Tile> edges;
  std::vector<Tile> inner;
  for (Tile& tile : tiles) {
    int num_unique = 0;
    for (std::uint16_t unique_edge : tile.GetEdgeStrips()) {
      if (edge_counts[unique_edge] == 1) ++num_unique;
    }
    switch (num_unique) {
      case 0:
        inner.emplace_back(std::move(tile));
        break;
      case 2:
        edges.emplace_back(std::move(tile));
        break;
      case 4:
        corners.emplace_back(std::move(tile));
        break;
      default:
        CHECK_FAIL();
    }
  }
  CHECK(corners.size() == 4);
  CHECK(edges.size() == (dim - 2) * 4);
  CHECK(inner.size() == (dim - 2) * (dim - 2));

  return {};
}

}  // namespace

int main(int argc, char** argv) {
  CHECK(2 == argc);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);
  std::vector<std::vector<std::string>> line_groups =
      aoc2020::SplitByEmptyStrings(std::move(lines));

  std::vector<Tile> tiles;
  for (const auto& line_group : line_groups) {
    tiles.emplace_back(line_group);
  }

  SolvePuzzle(std::move(tiles));

  return 0;
}
