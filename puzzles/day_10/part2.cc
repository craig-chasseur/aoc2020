#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "util/check.h"
#include "util/io.h"

namespace {

std::int64_t CountArrangements(const std::vector<int>& jolts) {
  absl::flat_hash_map<int, std::int64_t> jolts_suffixes;
  jolts_suffixes[jolts.back() + 3] = 1;
  for (auto iter = jolts.rbegin(); iter != jolts.rend(); ++iter) {
    jolts_suffixes[*iter] = jolts_suffixes[*iter + 1] +
                            jolts_suffixes[*iter + 2] +
                            jolts_suffixes[*iter + 3];
  }
  return jolts_suffixes[1] + jolts_suffixes[2] + jolts_suffixes[3];
}

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);
  std::vector<int> jolts = aoc2020::ParseIntegers(lines);

  std::sort(jolts.begin(), jolts.end());
  std::cout << CountArrangements(jolts) << "\n";

  return 0;
}
