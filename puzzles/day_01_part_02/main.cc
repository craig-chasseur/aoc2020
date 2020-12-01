#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "absl/status/statusor.h"
#include "absl/strings/numbers.h"
#include "io/io.h"

namespace {

std::vector<int> GetItems(const char* filename) {
  absl::StatusOr<std::vector<std::string>> lines = aoc2020::ReadLinesFromFile(filename);
  assert(lines.ok());
  std::vector<int> items;
  for (const std::string& line : *lines) {
    int parsed;
    const bool ok = absl::SimpleAtoi(line, &parsed);
    assert(ok);
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
  assert(false);
  return -1;
}

}  // namespace

int main(int argc, char** argv) {
  assert(argc == 2);
  std::vector<int> items = GetItems(argv[1]);
  std::cout << Solve(items) << "\n";
  return 0;
}
