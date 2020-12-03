#ifndef PUZZLES_DAY_03_TREE_MAP_H_
#define PUZZLES_DAY_03_TREE_MAP_H_

#include <cstdint>
#include <string>
#include <vector>

#include "absl/types/optional.h"

namespace aoc2020::day03 {

class TreeMap {
 public:
  explicit TreeMap(std::vector<std::string> map) : map_(std::move(map)) {}

  std::int64_t CountTreesOnPath(int step_down, int step_right) const;

 private:
  struct Coords {
    int vertical = 0;
    int horizontal = 0;
  };

  absl::optional<Coords> NextPosition(Coords original, int down,
                                      int right) const;

  bool IsTree(Coords coords) const {
    return map_[coords.vertical][coords.horizontal] == '#';
  }

  std::vector<std::string> map_;
};

}  // namespace aoc2020::day03

#endif  // PUZZLES_DAY_03_TREE_MAP_H_