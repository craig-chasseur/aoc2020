#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "absl/strings/string_view.h"
#include "util/check.h"
#include "util/io.h"

namespace {

unsigned ParseSeat(absl::string_view seat) {
  CHECK(seat.size() == 10);
  unsigned val = 0;
  for (char c : seat) {
    val <<= 1;
    switch (c) {
      case 'F':
      case 'L':
        val |= 0;
        break;
      case 'B':
      case 'R':
        val |= 1;
        break;
      default:
        CHECK_FAIL();
    }
  }
  return val;
}

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);

  std::vector<unsigned> seats;
  seats.reserve(lines.size());
  for (const absl::string_view line : lines) {
    seats.push_back(ParseSeat(line));
  }

  std::sort(seats.begin(), seats.end());
  for (auto iter = seats.begin() + 1; iter != seats.end(); ++iter) {
    if (*iter != *(iter - 1) + 1) {
      std::cout << (*iter - 1) << "\n";
      return 0;
    }
  }
  CHECK_FAIL();
}
