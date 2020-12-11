#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "absl/strings/string_view.h"
#include "util/check.h"
#include "util/io.h"

namespace {

class SeatMap {
 public:
  explicit SeatMap(std::vector<std::string> rep) : rep_(std::move(rep)) {}

  SeatMap Next() const {
    SeatMap next(rep_);
    for (int vertical = 0; vertical < rep_.size(); ++vertical) {
      for (int horizontal = 0; horizontal < rep_[vertical].size();
           ++horizontal) {
        if (rep_[vertical][horizontal] == 'L' &&
            CountVisible(vertical, horizontal) == 0) {
          next.rep_[vertical][horizontal] = '#';
        } else if (rep_[vertical][horizontal] == '#' &&
                   CountVisible(vertical, horizontal) >= 5) {
          next.rep_[vertical][horizontal] = 'L';
        }
      }
    }

    return next;
  }

  int CountOccupied() const {
    int total = 0;
    for (absl::string_view row : rep_) {
      total += std::count(row.begin(), row.end(), '#');
    }
    return total;
  }

  bool operator==(const SeatMap& other) const { return rep_ == other.rep_; }

 private:
  int CountVisible(const int vertical, const int horizontal) const {
    return SeatVisible(vertical, horizontal, -1, -1) +
           SeatVisible(vertical, horizontal, -1, 0) +
           SeatVisible(vertical, horizontal, -1, 1) +
           SeatVisible(vertical, horizontal, 0, -1) +
           SeatVisible(vertical, horizontal, 0, 1) +
           SeatVisible(vertical, horizontal, 1, -1) +
           SeatVisible(vertical, horizontal, 1, 0) +
           SeatVisible(vertical, horizontal, 1, 1);
  }

  int SeatVisible(int vertical_pos, int horizontal_pos,
                  const int vertical_delta, const int horizontal_delta) const {
    vertical_pos += vertical_delta;
    horizontal_pos += horizontal_delta;
    while (vertical_pos >= 0 && vertical_pos < rep_.size() &&
           horizontal_pos >= 0 && horizontal_pos < rep_[vertical_pos].size()) {
      if (rep_[vertical_pos][horizontal_pos] == '#') return 1;
      if (rep_[vertical_pos][horizontal_pos] == 'L') return 0;
      vertical_pos += vertical_delta;
      horizontal_pos += horizontal_delta;
    }
    return 0;
  }

  std::vector<std::string> rep_;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);

  SeatMap current(std::move(lines));
  for (;;) {
    SeatMap next = current.Next();
    if (current == next) {
      std::cout << current.CountOccupied() << "\n";
      break;
    }
    current = std::move(next);
  }

  return 0;
}
