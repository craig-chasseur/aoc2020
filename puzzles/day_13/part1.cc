#include <iostream>
#include <string>
#include <vector>

#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"

#include "util/check.h"
#include "util/io.h"

int main(int argc, char** argv) {
  CHECK(argc == 2);

  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);
  CHECK(lines.size() == 2);

  int earliest_time = 0;
  CHECK(absl::SimpleAtoi(lines.front(), &earliest_time));

  std::vector<int> candidate_busses;
  for (absl::string_view bus_id_str : absl::StrSplit(lines.back(), ',')) {
    int bus_id_parsed = 0;
    if (absl::SimpleAtoi(bus_id_str, &bus_id_parsed)) {
      candidate_busses.push_back(bus_id_parsed);
    } else {
      CHECK(bus_id_str == "x");
    }
  }

  for (int candidate_time = earliest_time;; ++candidate_time) {
    for (int bus_id : candidate_busses) {
      if (candidate_time % bus_id == 0) {
        std::cout << ((candidate_time - earliest_time) * bus_id) << "\n";
        return 0;
      }
    }
  }

  return 0;
}
