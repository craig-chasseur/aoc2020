#include <algorithm>
#include <iostream>

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
  auto maybe_lines = aoc2020::ReadLinesFromFile(argv[1]);
  CHECK_OK(maybe_lines);

  unsigned max_seat = 0;
  for (const absl::string_view line : *maybe_lines) {
    max_seat = std::max(max_seat, ParseSeat(line));
  }

  std::cout << max_seat << "\n";

  return 0;
}
