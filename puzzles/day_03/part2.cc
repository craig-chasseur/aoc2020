#include <cstdint>
#include <utility>

#include "puzzles/day_03/tree_map.h"
#include "util/check.h"
#include "util/io.h"

int main(int argc, char** argv) {
  CHECK(argc == 2);
  auto maybe_lines = aoc2020::ReadLinesFromFile(argv[1]);
  CHECK_OK(maybe_lines);

  aoc2020::day03::TreeMap map(std::move(maybe_lines).value());
  const std::int64_t product =
      map.CountTreesOnPath(1, 1) * map.CountTreesOnPath(1, 3) *
      map.CountTreesOnPath(1, 5) * map.CountTreesOnPath(1, 7) *
      map.CountTreesOnPath(2, 1);
  std::cout << product << "\n";
  return 0;
}
