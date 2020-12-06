#include <iostream>
#include <string>
#include <vector>

#include "util/check.h"
#include "util/io.h"

namespace {

int Solve(const std::vector<int>& items) {
  for (const int outer : items) {
    for (const int middle : items) {
      for (const int inner : items) {
        if (outer + middle + inner == 2020) {
          return outer * middle * inner;
        }
      }
    }
  }
  CHECK_FAIL();
}

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);

  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);
  std::vector<int> items = aoc2020::ParseIntegers(lines);

  std::cout << Solve(items) << "\n";
  return 0;
}
