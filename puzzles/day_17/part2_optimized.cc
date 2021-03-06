#if defined(__SSE2__)
#include <emmintrin.h>
#endif

#if defined(__AVX512BW__) || defined (__AVX2__)
#include <immintrin.h>
#endif

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "util/check.h"
#include "util/io.h"

// AVX512 is actually slower than AVX2 and SSE2 on my Skylake-X system, probably
// due to CPU frequency effects.
#define ENABLE_AVX512 0
#define ENABLE_AVX2 1
#define ENABLE_SSE2 1

namespace {

struct MiniCoords {
  std::int8_t x;
  std::int8_t y;
  std::int8_t z;
  std::int8_t w;
};

union CoordsRep {
  constexpr CoordsRep() : packed(0) {}

  explicit constexpr CoordsRep(std::uint32_t packed_in) : packed(packed_in) {}

  std::uint32_t packed;
  MiniCoords fields;
};

std::uint32_t InitCoords(std::int8_t x, std::int8_t y) {
  CoordsRep rep;
  rep.fields.x = x;
  rep.fields.y = y;
  return rep.packed;
}

class ScalarAdjacentCoords {
 public:
  explicit ScalarAdjacentCoords(std::uint32_t center) : center_(center) {}

  void InsertInSet(absl::flat_hash_set<std::uint32_t>* adjacent_set) const {
    for (int delta_x : {-1, 0, 1}) {
      for (int delta_y : {-1, 0, 1}) {
        for (int delta_z : {-1, 0, 1}) {
          for (int delta_w : {-1, 0, 1}) {
            if (delta_x != 0 || delta_y != 0 || delta_z != 0 || delta_w != 0) {
              CoordsRep rep(center_);
              rep.fields.x += delta_x;
              rep.fields.y += delta_y;
              rep.fields.z += delta_z;
              rep.fields.w += delta_w;
              adjacent_set->emplace(rep.packed);
            }
          }
        }
      }
    }
  }

  const std::uint32_t* begin() {
    if (adjacent_.empty()) Populate();
    return adjacent_.data();
  }

  const std::uint32_t* end() const {
    return adjacent_.data() + 80;
  }

 private:
  void Populate() {
    adjacent_.reserve(80);
    for (int delta_x : {-1, 0, 1}) {
      for (int delta_y : {-1, 0, 1}) {
        for (int delta_z : {-1, 0, 1}) {
          for (int delta_w : {-1, 0, 1}) {
            if (delta_x != 0 || delta_y != 0 || delta_z != 0 || delta_w != 0) {
              CoordsRep rep(center_);
              rep.fields.x += delta_x;
              rep.fields.y += delta_y;
              rep.fields.z += delta_z;
              rep.fields.w += delta_w;
              adjacent_.emplace_back(rep.packed);
            }
          }
        }
      }
    }
  }

  std::uint32_t center_;
  std::vector<std::uint32_t> adjacent_;
};

alignas(64) constexpr std::int8_t kDeltas[320]= {
    -1, -1, -1, -1,
    -1, -1, -1, 0,
    -1, -1, -1, 1,
    -1, -1, 0, -1,
    -1, -1, 0, 0,
    -1, -1, 0, 1,
    -1, -1, 1, -1,
    -1, -1, 1, 0,
    -1, -1, 1, 1,
    -1, 0, -1, -1,
    -1, 0, -1, 0,
    -1, 0, -1, 1,
    -1, 0, 0, -1,
    -1, 0, 0, 0,
    -1, 0, 0, 1,
    -1, 0, 1, -1,
    -1, 0, 1, 0,
    -1, 0, 1, 1,
    -1, 1, -1, -1,
    -1, 1, -1, 0,
    -1, 1, -1, 1,
    -1, 1, 0, -1,
    -1, 1, 0, 0,
    -1, 1, 0, 1,
    -1, 1, 1, -1,
    -1, 1, 1, 0,
    -1, 1, 1, 1,
    0, -1, -1, -1,
    0, -1, -1, 0,
    0, -1, -1, 1,
    0, -1, 0, -1,
    0, -1, 0, 0,
    0, -1, 0, 1,
    0, -1, 1, -1,
    0, -1, 1, 0,
    0, -1, 1, 1,
    0, 0, -1, -1,
    0, 0, -1, 0,
    0, 0, -1, 1,
    0, 0, 0, -1,
    0, 0, 0, 1,
    0, 0, 1, -1,
    0, 0, 1, 0,
    0, 0, 1, 1,
    0, 1, -1, -1,
    0, 1, -1, 0,
    0, 1, -1, 1,
    0, 1, 0, -1,
    0, 1, 0, 0,
    0, 1, 0, 1,
    0, 1, 1, -1,
    0, 1, 1, 0,
    0, 1, 1, 1,
    1, -1, -1, -1,
    1, -1, -1, 0,
    1, -1, -1, 1,
    1, -1, 0, -1,
    1, -1, 0, 0,
    1, -1, 0, 1,
    1, -1, 1, -1,
    1, -1, 1, 0,
    1, -1, 1, 1,
    1, 0, -1, -1,
    1, 0, -1, 0,
    1, 0, -1, 1,
    1, 0, 0, -1,
    1, 0, 0, 0,
    1, 0, 0, 1,
    1, 0, 1, -1,
    1, 0, 1, 0,
    1, 0, 1, 1,
    1, 1, -1, -1,
    1, 1, -1, 0,
    1, 1, -1, 1,
    1, 1, 0, -1,
    1, 1, 0, 0,
    1, 1, 0, 1,
    1, 1, 1, -1,
    1, 1, 1, 0,
    1, 1, 1, 1
};

template <typename VectorT>
class SimdTraits;

template <typename VectorT>
class SimdAdjacentCoords {
 private:
  static_assert(sizeof(kDeltas) % sizeof(VectorT) == 0);

  static inline constexpr std::size_t kElements =
      sizeof(kDeltas) / sizeof(VectorT);

 public:
  explicit SimdAdjacentCoords(std::uint32_t center) {
    const VectorT center_vec = SimdTraits<VectorT>::Splat(center);
    const VectorT* delta_vecs = reinterpret_cast<const VectorT*>(kDeltas);
    for (std::size_t i = 0; i < kElements; ++i) {
      adjacent_[i] = SimdTraits<VectorT>::AddI8(center_vec, delta_vecs[i]);
    }
  }

  void InsertInSet(absl::flat_hash_set<std::uint32_t>* adjacent_set) const {
    adjacent_set->insert(begin(), end());
  }

  const std::uint32_t* begin() const {
    return reinterpret_cast<const std::uint32_t*>(adjacent_);
  }

  const std::uint32_t* end() const {
    return reinterpret_cast<const std::uint32_t*>(adjacent_) + 80;
  }

 private:
  VectorT adjacent_[kElements];
};

#if ENABLE_AVX512 && defined(__AVX512BW__)

template <>
class SimdTraits<__m512i> {
 public:
  SimdTraits() = delete;

  static inline __m512i Splat(std::uint32_t value) {
    return _mm512_set1_epi32(value);
  }

  static inline __m512i AddI8(__m512i a, __m512i b) {
    return _mm512_add_epi8(a, b);
  }
};
using AdjacentCoords = SimdAdjacentCoords<__m512i>;

#elif ENABLE_AVX2 && defined(__AVX2__)

template <>
class SimdTraits<__m256i> {
 public:
  SimdTraits() = delete;

  static inline __m256i Splat(std::uint32_t value) {
    return _mm256_set1_epi32(value);
  }

  static inline __m256i AddI8(__m256i a, __m256i b) {
    return _mm256_add_epi8(a, b);
  }
};
using AdjacentCoords = SimdAdjacentCoords<__m256i>;

#elif ENABLE_SSE2 && defined(__SSE2__)

template <>
class SimdTraits<__m128i> {
 public:
  SimdTraits() = delete;

  static inline __m128i Splat(std::uint32_t value) {
    return _mm_set1_epi32(value);
  }

  static inline __m128i AddI8(__m128i a, __m128i b) {
    return _mm_add_epi8(a, b);
  }
};
using AdjacentCoords = SimdAdjacentCoords<__m128i>;

#else

using AdjacentCoords = ScalarAdjacentCoords;

#endif  // __AVX512BW__

class Grid {
 public:
  explicit Grid(const std::vector<std::string>& initial_state) {
    int y = 0;
    for (const std::string& line : initial_state) {
      int x = 0;
      for (const char c : line) {
        if (c == '#') {
          active_.emplace(InitCoords(x, y));
        }
        ++x;
      }
      ++y;
    }
  }

  void Step() {
    absl::flat_hash_set<std::uint32_t> candidate_active = active_;
    for (const std::uint32_t active_cube : active_) {
      AdjacentCoords(active_cube).InsertInSet(&candidate_active);
    }
    absl::erase_if(candidate_active, [this](const std::uint32_t candidate) {
      return !NextActive(candidate);
    });
    active_ = std::move(candidate_active);
  }

  int NumActive() const { return active_.size(); }

 private:
  int CountAdjacent(const std::uint32_t coords) const {
    int adjacent = 0;
    for (const std::uint32_t adjacent_cell : AdjacentCoords(coords)) {
      if (active_.contains(adjacent_cell)) {
        if (++adjacent == 4) return 4;
      }
    }
    return adjacent;
  }

  bool NextActive(const std::uint32_t coords) const {
    const int neighbors_active = CountAdjacent(coords);
    if (active_.contains(coords)) {
      return neighbors_active == 2 || neighbors_active == 3;
    }
    return neighbors_active == 3;
  }

  absl::flat_hash_set<std::uint32_t> active_;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK(2 == argc);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);

  Grid grid(lines);
  for (int i = 0; i < 6; ++i) {
    grid.Step();
  }
  std::cout << grid.NumActive() << "\n";

  return 0;
}
