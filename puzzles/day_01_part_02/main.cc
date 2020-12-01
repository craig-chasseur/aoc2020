#include <iostream>
#include <string>
#include <vector>

#include "absl/status/statusor.h"
#include "absl/strings/numbers.h"
#include "util/check.h"
#include "util/io.h"

namespace {

std::vector<int> GetItems(const char* filename) {
  absl::StatusOr<std::vector<std::string>> lines =
      aoc2020::ReadLinesFromFile(filename);
  CHECK_OK(lines);
  std::vector<int> items;
  for (const std::string& line : *lines) {
    int parsed;
    CHECK(absl::SimpleAtoi(line, &parsed));
    items.push_back(parsed);
  }
  return items;
}

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
  std::vector<int> items = GetItems(argv[1]);
  std::cout << Solve(items) << "\n";
  return 0;
}
