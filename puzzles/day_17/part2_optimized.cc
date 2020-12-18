#ifdef __AVX512BW__
#include <immintrin.h>
#endif

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "util/check.h"
#include "util/io.h"

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

#ifdef __AVX512BW__

class AdjacentCoords {
 public:
  explicit AdjacentCoords(std::uint32_t center) {
    const __m512i center_vec = _mm512_set1_epi32(center);
    adjacent_[0] = _mm512_add_epi8(center_vec, kDeltas[0]);
    adjacent_[1] = _mm512_add_epi8(center_vec, kDeltas[1]);
    adjacent_[2] = _mm512_add_epi8(center_vec, kDeltas[2]);
    adjacent_[3] = _mm512_add_epi8(center_vec, kDeltas[3]);
    adjacent_[4] = _mm512_add_epi8(center_vec, kDeltas[4]);
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
  static inline const __m512i kDeltas[5] = {
      _mm512_set_epi8(-1, -1, -1, -1,
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
                      -1, 0, 1, -1),
      _mm512_set_epi8(-1, 0, 1, 0,
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
                      0, -1, 0, 0),
      _mm512_set_epi8(0, -1, 0, 1,
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
                      0, 1, 0, -1),
      _mm512_set_epi8(0, 1, 0, 0,
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
                      1, 0, -1, 0),
      _mm512_set_epi8(1, 0, -1, 1,
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
                      1, 1, 1, 1)};

  __m512i adjacent_[5];
};

#else  // !__AVX512BW__

class AdjacentCoords {
 public:
  explicit AdjacentCoords(std::uint32_t center) : center_(center) {}

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

  const absl::Time pre = absl::Now();
  for (int i = 0; i < 10; ++i) {
    Grid grid(lines);
    for (int i = 0; i < 6; ++i) {
      grid.Step();
    }
    CHECK(1620 == grid.NumActive());
  }
  const absl::Time post = absl::Now();
  std::cout << (post - pre) << "\n";

  return 0;
}
