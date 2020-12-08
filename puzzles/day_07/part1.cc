#include <queue>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/string_view.h"
#include "puzzles/day_07/rule_parser.h"
#include "util/check.h"
#include "util/io.h"

namespace {

class BagGraph {
 public:
  void AddRule(const aoc2020::BagRule& rule) {
    for (const aoc2020::ContainedBags& contained : rule.contained_bags) {
      contained_by_[contained.color].emplace_back(
          std::string(rule.container_color));
    }
  }

  int CountOutermostBags(std::string bag_color) const {
    absl::flat_hash_set<std::string> outermost_bags;
    std::queue<std::string> unprocessed_bags;
    unprocessed_bags.push(bag_color);
    while (!unprocessed_bags.empty()) {
      const std::string& current_bag = unprocessed_bags.front();
      if (outermost_bags.insert(current_bag).second) {
        auto iter = contained_by_.find(current_bag);
        if (iter != contained_by_.end()) {
          for (const std::string& container : iter->second) {
            unprocessed_bags.push(container);
          }
        }
      }
      unprocessed_bags.pop();
    }

    return outermost_bags.size() - 1;
  }

 private:
  absl::flat_hash_map<std::string, std::vector<std::string>> contained_by_;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);

  BagGraph graph;
  for (const absl::string_view line : lines) {
    graph.AddRule(aoc2020::ParseBagRule(line));
  }

  std::cout << graph.CountOutermostBags("shiny gold") << "\n";

  return 0;
}
