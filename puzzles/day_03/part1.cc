#include <string>
#include <utility>
#include <vector>

#include "puzzles/day_03/tree_map.h"
#include "util/check.h"
#include "util/io.h"

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);

  aoc2020::day03::TreeMap map(std::move(lines));
  std::cout << map.CountTreesOnPath(1, 3) << "\n";
  return 0;
}
