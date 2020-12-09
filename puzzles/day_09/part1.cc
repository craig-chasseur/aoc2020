#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "absl/strings/numbers.h"
#include "puzzles/day_09/xmas_cipher.h"
#include "util/check.h"
#include "util/io.h"

int main(int argc, char** argv) {
  CHECK(3 == argc);

  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);
  std::vector<std::int64_t> numbers =
      aoc2020::ParseIntegers<std::int64_t>(lines);

  int window_size = 0;
  CHECK(absl::SimpleAtoi(argv[2], &window_size));

  std::cout << aoc2020::xmas_cipher::FindInvalidNumber(numbers, window_size)
            << "\n";

  return 0;
}
