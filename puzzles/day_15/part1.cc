#include <algorithm>
#include <iostream>
#include <vector>

#include "util/check.h"

namespace {

int NthNumber(std::vector<int> sequence, const int n) {
  while (sequence.size() < n) {
    auto iter =
        std::find(sequence.rbegin() + 1, sequence.rend(), sequence.back());
    if (iter == sequence.rend()) {
      sequence.push_back(0);
    } else {
      sequence.push_back(iter - sequence.rbegin());
    }
  }
  return sequence.back();
}

}  // namespace

int main(int argc, char** argv) {
  const std::vector<int> inputs{0, 5, 4, 1, 10, 14, 7};
  std::cout << NthNumber(inputs, 2020) << "\n";
  return 0;
}
