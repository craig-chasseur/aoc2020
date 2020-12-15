#include <algorithm>
#include <iostream>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "util/check.h"

namespace {

// This solution uses about 32 MiB of memory and 13 seconds of CPU. It's alright
// but we can probably do better.
int NthNumber(std::vector<int> sequence, const int n) {
  absl::flat_hash_map<int, int> number_to_position;
  for (int idx = 0; idx < sequence.size() - 1; ++idx) {
    number_to_position[sequence[idx]] = idx;
  }

  int back = sequence.back();
  for (int counter = number_to_position.size(); counter < n - 1; ++counter) {
    auto iter = number_to_position.find(back);
    int next_back = 0;
    if (iter != number_to_position.end()) {
      next_back = counter - iter->second;
    }
    number_to_position[back] = counter;
    back = next_back;
  }

  return back;
}

}  // namespace

int main(int argc, char** argv) {
  const std::vector<int> inputs{0, 5, 4, 1, 10, 14, 7};
  std::cout << NthNumber(inputs, 30000000) << "\n";
  return 0;
}
