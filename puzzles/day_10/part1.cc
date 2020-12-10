#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "util/check.h"
#include "util/io.h"

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);
  std::vector<int> jolts = aoc2020::ParseIntegers(lines);

  std::sort(jolts.begin(), jolts.end());

  int one_diffs = 0;
  int three_diffs = 0;
  switch (jolts.front()) {
    case 1:
      ++one_diffs;
      break;
    case 3:
      ++three_diffs;
      break;
  }

  for (auto iter = jolts.begin() + 1; iter != jolts.end(); ++iter) {
    switch (*iter - *(iter - 1)) {
      case 1:
        ++one_diffs;
        break;
      case 3:
        ++three_diffs;
        break;
    }
  }
  ++three_diffs;

  std::cout << (one_diffs * three_diffs) << "\n";

  return 0;
}
