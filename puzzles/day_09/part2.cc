#include <algorithm>
#include <cstdint>
#include <deque>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "absl/strings/numbers.h"
#include "util/check.h"
#include "util/io.h"

namespace {

class XmasWindow {
 public:
  explicit XmasWindow(const int window_size) : window_size_(window_size) {}

  std::int64_t CrackCode(const std::vector<std::int64_t>& sequence) {
    const std::int64_t invalid = FindFirstInvalid(sequence);
    for (auto range_start = sequence.begin(); range_start != sequence.end();
         ++range_start) {
      for (auto range_end = range_start; range_end != sequence.end();
           ++range_end) {
        const std::int64_t sum =
            std::accumulate(range_start, range_end, std::int64_t{0});
        if (sum == invalid) {
          return SumMinMax(range_start, range_end);
        }
        if (sum > invalid) break;
      }
    }
    CHECK_FAIL();
  }

  std::int64_t FindFirstInvalid(const std::vector<std::int64_t>& sequence) {
    CHECK(sequence.size() >= window_size_);
    window_.assign(sequence.begin(), sequence.begin() + window_size_);
    for (auto iter = sequence.begin() + window_size_; iter != sequence.end();
         ++iter) {
      if (!NextValid(*iter)) return *iter;
      window_.pop_front();
      window_.push_back(*iter);
    }
    CHECK_FAIL();
  }

 private:
  bool NextValid(const std::int64_t next) const {
    for (auto outer = window_.begin(); outer != window_.end(); ++outer) {
      for (auto inner = window_.begin(); inner != window_.end(); ++inner) {
        if (outer == inner) continue;
        if ((*outer + *inner) == next) return true;
      }
    }
    return false;
  }

  static int64_t SumMinMax(std::vector<std::int64_t>::const_iterator begin,
                           std::vector<std::int64_t>::const_iterator end) {
    return *std::min_element(begin, end) + *std::max_element(begin, end);
  }

  int window_size_ = 0;
  std::deque<std::int64_t> window_;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK(3 == argc);

  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);
  std::vector<std::int64_t> numbers =
      aoc2020::ParseIntegers<std::int64_t>(lines);

  int window_size = 0;
  CHECK(absl::SimpleAtoi(argv[2], &window_size));

  XmasWindow window(window_size);
  std::cout << window.CrackCode(numbers) << "\n";

  return 0;
}
