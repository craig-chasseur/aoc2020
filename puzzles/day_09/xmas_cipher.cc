#include "puzzles/day_09/xmas_cipher.h"

#include <algorithm>
#include <cstdint>
#include <deque>

#include "absl/types/span.h"
#include "util/check.h"

namespace aoc2020::xmas_cipher {
namespace {

bool NextValid(const std::deque<std::int64_t>& window,
               const std::int64_t next) {
  for (auto outer = window.begin(); outer != window.end(); ++outer) {
    for (auto inner = window.begin(); inner != window.end(); ++inner) {
      if (outer == inner) continue;
      if ((*outer + *inner) == next) return true;
    }
  }
  return false;
}

}  // namespace

std::int64_t FindInvalidNumber(absl::Span<const std::int64_t> sequence,
                               const int window_size) {
  CHECK(sequence.size() >= window_size);
  std::deque<std::int64_t> window(sequence.begin(),
                                  sequence.begin() + window_size);
  for (auto iter = sequence.begin() + window_size; iter != sequence.end();
       ++iter) {
    if (!NextValid(window, *iter)) return *iter;
    window.pop_front();
    window.push_back(*iter);
  }
  CHECK_FAIL();
}

std::int64_t CrackCode(absl::Span<const std::int64_t> sequence,
                       const int window_size) {
  const std::int64_t target = FindInvalidNumber(sequence, window_size);

  std::int64_t current_sum = 0;
  auto range_begin = sequence.begin();
  auto range_end = sequence.begin();
  while (range_end != sequence.end()) {
    if (current_sum == target) {
      auto [min, max] = std::minmax_element(range_begin, range_end);
      return *min + *max;
    }

    if (current_sum < target) {
      current_sum += *range_end;
      ++range_end;
      continue;
    }

    CHECK(current_sum > target);
    current_sum -= *range_begin;
    ++range_begin;
  }

  CHECK_FAIL();
}

}  // namespace aoc2020::xmas_cipher
