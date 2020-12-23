#include <deque>
#include <vector>

#include "absl/strings/str_join.h"
#include "absl/types/span.h"
#include "util/check.h"

namespace {

class Cups {
 public:
  explicit Cups(absl::Span<const int> cups) : cups_(cups.begin(), cups.end()) {}

  void Move() {
    const std::vector<int> removed_cups(cups_.begin() + 1, cups_.begin() + 4);
    cups_.erase(cups_.begin() + 1, cups_.begin() + 4);

    const int current = cups_.front();
    std::deque<int>::const_iterator highest_below_current = cups_.end();
    std::deque<int>::const_iterator highest_overall = cups_.end();
    for (auto iter = cups_.begin(); iter != cups_.end(); ++iter) {
      if (*iter < current) {
        if (highest_below_current == cups_.end() ||
            *iter > *highest_below_current) {
          highest_below_current = iter;
        }
      }
      if (highest_overall == cups_.end() || *iter > *highest_overall) {
        highest_overall = iter;
      }
    }
    CHECK(highest_overall != cups_.end());
    const std::deque<int>::const_iterator destination_iter =
        (highest_below_current != cups_.end()) ? highest_below_current
                                               : highest_overall;

    cups_.insert(destination_iter + 1, removed_cups.begin(),
                 removed_cups.end());

    cups_.pop_front();
    cups_.push_back(current);
  }

  std::string OrderString() const {
    auto one_iter = std::find(cups_.begin(), cups_.end(), 1);
    return absl::StrJoin(one_iter + 1, cups_.end(), "") +
           absl::StrJoin(cups_.begin(), one_iter, "");
  }

 private:
  std::deque<int> cups_;
};

constexpr int kTestInput[] = {3, 8, 9, 1, 2, 5, 4, 6, 7};
constexpr int kRealInput[] = {1, 5, 6, 7, 9, 4, 8, 2, 3};

}  // namespace

int main(int argc, char** argv) {
  Cups cups(kRealInput);
  for (int i = 0; i < 100; ++i) {
    cups.Move();
  }
  std::cout << cups.OrderString() << "\n";

  return 0;
}
