#include <cmath>
#include <cstdint>
#include <cstdio>
#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "util/check.h"
#include "util/io.h"

namespace {

std::uint16_t Reverse10(std::uint16_t input) {
  std::uint16_t reversed = 0;
  for (int i = 0; i < 10; ++i) {
    if ((input & (1 << i)) != 0) {
      reversed |= (1 << (9 - i));
    }
  }
  return reversed;
}

class Tile {
 public:
  explicit Tile(const std::vector<std::string>& line_group) {
    CHECK(line_group.size() == 11);
    CHECK(1 == std::sscanf(line_group.front().c_str(), "Tile %d:", &id_));
    for (int row_idx = 0; row_idx < 10; ++row_idx) {
      std::uint16_t parsed_row = 0;
      for (char c : line_group[row_idx + 1]) {
        parsed_row <<= 1;
        if (c == '#') {
          parsed_row |= 1;
        } else {
          CHECK(c == '.');
        }
      }
      image_[row_idx] = Reverse10(parsed_row);
    }
  }

  int id() const { return id_; }

  std::vector<std::uint16_t> GetEdgeStrips() const {
    return {image_[0],
            image_[9],
            VerticalStripe(0),
            VerticalStripe(9),
            Reverse10(image_[0]),
            Reverse10(image_[9]),
            Reverse10(VerticalStripe(0)),
            Reverse10(VerticalStripe(9))};
  }

 private:
  std::uint16_t VerticalStripe(const int horizontal_position) const {
    std::uint16_t stripe = 0;
    for (int i = 0; i < 10; ++i) {
      if ((image_[i] & (1 << horizontal_position)) != 0) {
        stripe |= (1 << i);
      }
    }
    return stripe;
  }

  int id_ = 0;
  std::uint16_t image_[10] = {};
};

std::vector<int> FindCorners(std::vector<Tile> tiles) {
  absl::flat_hash_map<std::uint16_t, int> edge_counts;
  for (const Tile& tile : tiles) {
    for (std::uint16_t edge : tile.GetEdgeStrips()) {
      ++edge_counts[edge];
    }
  }

  std::vector<int> corners;
  for (const Tile& tile : tiles) {
    int num_unique = 0;
    for (std::uint16_t unique_edge : tile.GetEdgeStrips()) {
      if (edge_counts[unique_edge] == 1) ++num_unique;
    }
    if (num_unique == 4) corners.push_back((tile.id()));
  }
  CHECK(corners.size() == 4);
  return corners;
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

  std::vector<int> corners = FindCorners(std::move(tiles));
  std::int64_t product = 1;
  for (std::int64_t id : corners) {
    product *= id;
  }
  std::cout << product << "\n";

  return 0;
}
