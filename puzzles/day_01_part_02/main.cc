#include <iostream>
#include <string>
#include <vector>

#include "absl/status/statusor.h"
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

  absl::StatusOr<std::vector<std::string>> maybe_lines =
      aoc2020::ReadLinesFromFile(argv[1]);
  CHECK_OK(maybe_lines);
  absl::StatusOr<std::vector<int>> maybe_items =
      aoc2020::ParseIntegers(*maybe_lines);
  CHECK_OK(maybe_items);

  std::cout << Solve(*maybe_items) << "\n";
  return 0;
}
