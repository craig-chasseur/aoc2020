#ifndef PUZZLES_DAY_20_BIT_SQUARE_H_
#define PUZZLES_DAY_20_BIT_SQUARE_H_

#include <cstddef>
#include <cstdint>
#include <vector>

#include "util/check.h"

namespace {

class BitSquare {
 public:
  BitSquare() = default;

  explicit BitSquare(std::size_t dim)
      : dim_(dim), rep_((dim * dim + 63) / 64, 0) {}

  std::size_t dim() const { return dim_; }

  void Set(std::size_t y, std::size_t x, bool value) {
    const std::size_t offset = Offset(y, x);
    if (value) {
      rep_[offset >> kHighShift] |= (std::uint64_t{1} << (offset & kLowMask));
    } else {
      rep_[offset >> kHighShift] &= ~(std::uint64_t{1} << (offset & kLowMask));
    }
  }

  bool Get(std::size_t y, std::size_t x) const {
    const std::size_t offset = Offset(y, x);
    return (rep_[offset >> kHighShift] &
            (std::uint64_t{1} << (offset & kLowMask))) != 0;
  }

  std::uint16_t SliceHorizontal(std::size_t y) const {
    CHECK(dim_ <= 16);
    std::uint16_t slice = 0;
    for (std::size_t i = 0; i < dim_; ++i) {
      slice |= (1 << i) * Get(y, i);
    }
    return slice;
  }

  std::uint16_t SliceVertical(std::size_t x) const {
    CHECK(dim_ <= 16);
    std::uint16_t slice = 0;
    for (std::size_t i = 0; i < dim_; ++i) {
      slice |= (1 << i) * Get(i, x);
    }
    return slice;
  }

  BitSquare RotateClockwise() const {
    BitSquare rotated(dim_);
    for (std::size_t y = 0; y < dim_; ++y) {
      for (std::size_t x = 0; x < dim_; ++x) {
        rotated.Set(x, dim_ - y - 1, Get(y, x));
      }
    }
    return rotated;
  }

  BitSquare FlipHorizonal() const {
    BitSquare flipped(dim_);
    for (std::size_t y = 0; y < dim_; ++y) {
      for (std::size_t x = 0; x < dim_; ++x) {
        flipped.Set(y, dim_ - x - 1, Get(y, x));
      }
    }
    return flipped;
  }

  BitSquare FlipVertical() const {
    BitSquare flipped(dim_);
    for (std::size_t y = 0; y < dim_; ++y) {
      for (std::size_t x = 0; x < dim_; ++x) {
        flipped.Set(dim_ - y - 1, x, Get(y, x));
      }
    }
    return flipped;
  }

  std::vector<BitSquare> AllOrientations() const {
    std::vector<BitSquare> all_orientations;
    all_orientations.reserve(8);
    BitSquare base = *this;
    // Rotate 180 is equivalent to flipping both horizontal and vertical, so we
    // only need to do one rotation in combination with all possible flips.
    for (int rotations = 0; rotations < 2; ++rotations) {
      all_orientations.emplace_back(base);
      all_orientations.emplace_back(base.FlipHorizonal());
      all_orientations.emplace_back(base.FlipVertical());
      all_orientations.emplace_back(base.FlipHorizonal().FlipVertical());
      base = base.RotateClockwise();
    }
    return all_orientations;
  }

  std::size_t CountOnes() const {
    std::size_t total = 0;
    for (const std::uint64_t elem : rep_) {
      total += __builtin_popcountll(elem);
    }
    return total;
  }

 private:
  static inline constexpr std::size_t kHighShift = 6;
  static inline constexpr std::size_t kLowMask = 63;

  std::size_t Offset(std::size_t y, std::size_t x) const {
    return y * dim_ + x;
  }

  std::size_t dim_ = 0;
  std::vector<std::uint64_t> rep_;
};

}  // namespace

#endif  // PUZZLES_DAY_20_BIT_SQUARE_H_
