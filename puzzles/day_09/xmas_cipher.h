#ifndef PUZZLES_DAY_09_XMAS_CIPHER_H_
#define PUZZLES_DAY_09_XMAS_CIPHER_H_

#include <cstdint>

#include "absl/types/span.h"

namespace aoc2020::xmas_cipher {

std::int64_t FindInvalidNumber(absl::Span<const std::int64_t> sequence,
                               int window_size);

std::int64_t CrackCode(absl::Span<const std::int64_t> sequence,
                       int window_size);

}  // namespace aoc2020::xmas_cipher

#endif  // PUZZLES_DAY_09_XMAS_CIPHER_H_
