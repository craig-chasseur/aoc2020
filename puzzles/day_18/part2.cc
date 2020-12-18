#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "absl/strings/string_view.h"
#include "puzzles/day_18/expr.h"
#include "puzzles/day_18/parse.h"
#include "util/check.h"
#include "util/io.h"

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);

  std::int64_t sum = 0;
  for (absl::string_view line : lines) {
    sum +=
        aoc2020::newmath::ParseFull(line, /*add_has_precedence=*/true)->Eval();
  }
  std::cout << sum << "\n";

  return 0;
}
