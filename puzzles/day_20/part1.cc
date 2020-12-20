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

struct Edges {
  std::uint16_t top = 0;
  std::uint16_t bottom = 0;
  std::uint16_t left = 0;
  std::uint16_t right = 0;

  Edges Rotate() const {
    return Edges{.top = Reverse10(left),
                 .bottom = Reverse10(right),
                 .left = bottom,
                 .right = top};
  }

  Edges FlipVertical() const {
    return Edges{.top = bottom, .bottom = top, .left = left, .right = right};
  }

  Edges FlipHorizonal() const {
    return Edges{.top = top, .bottom = bottom, .left = right, .right = left};
  }

  template <typename H>
  friend H AbslHashValue(H h, const Edges& edges) {
    return H::combine(std::move(h), edges.top, edges.bottom, edges.left,
                      edges.right);
  }

  bool operator==(const Edges& other) const {
    return top == other.top && bottom == other.bottom && left == other.left &&
           right == other.right;
  }
};

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

  std::vector<Edges> GetEdges() const {
    Edges initial{.top = image_[0],
                  .bottom = image_[9],
                  .left = VerticalStripe(0),
                  .right = VerticalStripe(9)};
    absl::flat_hash_set<Edges> all_edges;
    for (int rotations = 0; rotations < 4; ++rotations) {
      all_edges.insert(initial);
      all_edges.insert(initial.FlipHorizonal());
      all_edges.insert(initial.FlipVertical());
      all_edges.insert(initial.FlipHorizonal().FlipVertical());
      initial = initial.Rotate();
    }
    return {all_edges.begin(), all_edges.end()};
  }

  void AddCandidateRight(int id) { right_ids_.insert(id); }

  const absl::flat_hash_set<int>& right_candidates() const {
    return right_ids_;
  }

  void AddCandidateBelow(int id) { below_ids_.insert(id); }

  const absl::flat_hash_set<int>& below_candidates() const {
    return below_ids_;
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
  absl::flat_hash_set<int> right_ids_;
  absl::flat_hash_set<int> below_ids_;
};

bool SolveImpl(const absl::flat_hash_map<int, const Tile*>& id_to_tile,
               std::vector<std::vector<int>>* assignments, int y, int x,
               absl::flat_hash_set<int>* unassigned_set) {
  if (unassigned_set->empty()) return true;

  absl::flat_hash_set<int> candidates = *unassigned_set;
  if (x > 0) {
    const Tile* left = id_to_tile.at((*assignments)[y][x - 1]);
    absl::erase_if(candidates, [left](int id) {
      return !left->right_candidates().contains(id);
    });
    if (candidates.empty()) return false;
  }
  if (y > 0) {
    const Tile* above = id_to_tile.at((*assignments)[y - 1][x]);
    absl::erase_if(candidates, [above](int id) {
      return !above->below_candidates().contains(id);
    });
    if (candidates.empty()) return false;
  }

  int next_x = x;
  int next_y = y;
  if (++next_x == assignments->at(y).size()) {
    ++next_y;
    next_x = 0;
  }

  for (const int candidate : candidates) {
    (*assignments)[y][x] = candidate;
    CHECK(unassigned_set->erase(candidate) == 1);
    if (SolveImpl(id_to_tile, assignments, next_y, next_x, unassigned_set)) {
      return true;
    }
    unassigned_set->insert(candidate);
  }
  return false;
}

std::vector<std::vector<int>> Solve(std::vector<Tile> tiles) {
  for (int candidate_tile_idx = 0; candidate_tile_idx < tiles.size();
       ++candidate_tile_idx) {
    Tile& candidate_tile = tiles[candidate_tile_idx];
    for (int other_tile_idx = 0; other_tile_idx < tiles.size();
         ++other_tile_idx) {
      if (other_tile_idx == candidate_tile_idx) continue;
      const Tile& other_tile = tiles[other_tile_idx];
      for (const Edges& edges : candidate_tile.GetEdges()) {
        for (const Edges& other_edges : other_tile.GetEdges()) {
          if (edges.right == other_edges.left) {
            candidate_tile.AddCandidateRight(other_tile.id());
          }
          if (edges.bottom == other_edges.top) {
            candidate_tile.AddCandidateBelow(other_tile.id());
          }
        }
      }
    }
  }

  absl::flat_hash_map<int, const Tile*> id_to_tile;
  absl::flat_hash_set<int> unassigned_set;
  for (const Tile& tile : tiles) {
    CHECK(id_to_tile.try_emplace(tile.id(), &tile).second);
    CHECK(unassigned_set.insert(tile.id()).second);
  }

  const int dim = std::sqrt(tiles.size());
  std::vector<std::vector<int>> assignments(dim, std::vector<int>(dim, 0));

  CHECK(SolveImpl(id_to_tile, &assignments, 0, 0, &unassigned_set));
  return assignments;
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

  std::vector<std::vector<int>> solved = Solve(std::move(tiles));
  const std::int64_t product =
      static_cast<std::int64_t>(solved.front().front()) *
      static_cast<std::int64_t>(solved.front().back()) *
      static_cast<std::int64_t>(solved.back().front()) *
      static_cast<std::int64_t>(solved.back().back());
  std::cout << "\n" << product << "\n";

  return 0;
}
