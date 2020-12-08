#include <queue>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/string_view.h"
#include "puzzles/day_07/rule_parser.h"
#include "util/check.h"
#include "util/io.h"

namespace {

class BagGraph {
 public:
  void AddRule(aoc2020::BagRule rule) {
    CHECK(contains_
              .emplace(std::string(rule.container_color),
                       std::move(rule.contained_bags))
              .second);
  }

  int CountHeldBags(absl::string_view bag_color) const {
    auto iter = contains_.find(bag_color);
    int total_bags = 1;
    for (const aoc2020::ContainedBags& held : iter->second) {
      total_bags += held.count * CountHeldBags(held.color);
    }
    return total_bags;
  }

 private:
  absl::flat_hash_map<std::string, std::vector<aoc2020::ContainedBags>>
      contains_;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);

  BagGraph graph;
  for (const absl::string_view line : lines) {
    graph.AddRule(aoc2020::ParseBagRule(line));
  }

  std::cout << (graph.CountHeldBags("shiny gold") - 1) << "\n";

  return 0;
}
