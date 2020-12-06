#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "absl/strings/string_view.h"
#include "util/check.h"
#include "util/io.h"

namespace {

class Group {
 public:
  explicit Group(const std::vector<std::string>& group_str) {
    for (const absl::string_view line : group_str) {
      std::uint32_t person_answers = 0;
      for (const char c : line) {
        const int pos = c - 'a';
        person_answers |= (1 << pos);
      }
      answers_ &= person_answers;
    }
  }

  int NumAnswers() const {
    return __builtin_popcount(answers_);
  }

 private:
  std::uint32_t answers_ = std::numeric_limits<std::uint32_t>::max();
};

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);
  std::vector<std::vector<std::string>> groups =
      aoc2020::SplitByEmptyStrings(std::move(lines));

  int total = 0;
  for (const auto& group_str : groups) {
    total += Group(group_str).NumAnswers();
  }
  std::cout << total << "\n";
  return 0;
}
