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
            CountAdjacent(vertical, horizontal) == 0) {
          next.rep_[vertical][horizontal] = '#';
        } else if (rep_[vertical][horizontal] == '#' &&
                   CountAdjacent(vertical, horizontal) >= 4) {
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
  int CountAdjacent(const int vertical, const int horizontal) const {
    int count = 0;
    if (vertical > 0) {
      if (horizontal > 0) {
        count += (rep_[vertical - 1][horizontal - 1] == '#');
      }
      count += (rep_[vertical - 1][horizontal] == '#');
      if (horizontal < rep_[vertical - 1].size() - 1) {
        count += (rep_[vertical - 1][horizontal + 1] == '#');
      }
    }

    if (horizontal > 0) {
      count += (rep_[vertical][horizontal - 1] == '#');
    }
    if (horizontal < rep_[vertical].size() - 1) {
      count += (rep_[vertical][horizontal + 1] == '#');
    }

    if (vertical < rep_.size() - 1) {
      if (horizontal > 0) {
        count += (rep_[vertical + 1][horizontal - 1] == '#');
      }
      count += (rep_[vertical + 1][horizontal] == '#');
      if (horizontal < rep_[vertical + 1].size() - 1) {
        count += (rep_[vertical + 1][horizontal + 1] == '#');
      }
    }

    return count;
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
