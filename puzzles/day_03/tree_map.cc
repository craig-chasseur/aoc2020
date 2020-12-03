#include "puzzles/day_03/tree_map.h"

#include <cstdint>

#include "absl/types/optional.h"

namespace aoc2020::day03 {

std::int64_t TreeMap::CountTreesOnPath(const int step_down,
                                       const int step_right) const {
  absl::optional<Coords> position = Coords();
  std::int64_t trees = 0;
  while (position.has_value()) {
    trees += IsTree(*position);
    position = NextPosition(*position, step_down, step_right);
  }
  return trees;
}

absl::optional<TreeMap::Coords> TreeMap::NextPosition(Coords original,
                                                      const int down,
                                                      const int right) const {
  original.vertical += down;
  if (original.vertical >= map_.size()) return absl::nullopt;

  original.horizontal += right;
  original.horizontal %= map_[original.vertical].size();

  return original;
}

}  // namespace aoc2020::day03
