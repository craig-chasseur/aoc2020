#include <algorithm>
#include <cstdint>
#include <list>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/types/span.h"
#include "util/check.h"

namespace {

template <typename Iter>
Iter Advance(Iter iter, const int n) {
  for (int i = 0; i < n; ++i) {
    ++iter;
  }
  return iter;
}

class Cups {
 public:
  explicit Cups(absl::Span<const int> cups)
      : cups_(cups.begin(), cups.end()),
        max_cup_(*std::max_element(cups.begin(), cups.end())) {
    lookup_table_.reserve(cups_.size());
    for (auto iter = cups_.begin(); iter != cups_.end(); ++iter) {
      lookup_table_.try_emplace(*iter, iter);
    }
  }

  void Move() {
    std::list<int> removed_cups;
    removed_cups.splice(removed_cups.begin(), cups_, Advance(cups_.begin(), 1),
                        Advance(cups_.begin(), 4));

    const int current = cups_.front();
    const std::list<int>::const_iterator destination_iter =
        FindDestination(current, removed_cups);

    cups_.splice(Advance(destination_iter, 1), std::move(removed_cups));
    cups_.splice(cups_.end(), cups_, cups_.begin());
  }

  std::int64_t AfterOneProduct() const {
    auto one_iter = lookup_table_.find(1)->second;
    std::int64_t product = 1;

    if (++one_iter == cups_.end()) one_iter = cups_.begin();
    product *= static_cast<std::int64_t>(*one_iter);

    if (++one_iter == cups_.end()) one_iter = cups_.begin();
    product *= static_cast<std::int64_t>(*one_iter);

    return product;
  }

 private:
  std::list<int>::const_iterator FindDestination(
      int current_value, const std::list<int>& removed_cups) const {
    for (;;) {
      if (--current_value == 0) {
        current_value = max_cup_;
      }
      if (std::find(removed_cups.begin(), removed_cups.end(), current_value) !=
          removed_cups.end()) {
        continue;
      }
      auto lookup_table_iter = lookup_table_.find(current_value);
      CHECK(lookup_table_iter != lookup_table_.end());
      return lookup_table_iter->second;
    }
  }

  std::list<int> cups_;
  int max_cup_ = 0;
  absl::flat_hash_map<int, std::list<int>::const_iterator> lookup_table_;
};

constexpr int kTestInput[] = {3, 8, 9, 1, 2, 5, 4, 6, 7};
constexpr int kRealInput[] = {1, 5, 6, 7, 9, 4, 8, 2, 3};

}  // namespace

int main(int argc, char** argv) {
  std::vector<int> all_input(kRealInput, kRealInput + 9);
  for (int i = 10; i <= 1'000'000; ++i) {
    all_input.push_back(i);
  }
  CHECK(1'000'000 == all_input.size());

  Cups cups(all_input);
  for (int i = 0; i < 10'000'000; ++i) {
    cups.Move();
  }
  std::cout << cups.AfterOneProduct() << "\n";

  return 0;
}
