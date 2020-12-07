#include <queue>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/match.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "re2/re2.h"
#include "util/check.h"
#include "util/io.h"

namespace {

class BagGraph {
 public:
  void AddRule(absl::string_view rule) {
    CHECK(rule.back() == '.');
    rule.remove_suffix(1);
    std::vector<absl::string_view> rule_parts = absl::StrSplit(rule, ", ");

    absl::string_view rule_head = rule_parts.front();
    std::vector<absl::string_view> head_clauses =
        absl::StrSplit(rule_head, " bags contain ");
    CHECK(head_clauses.size() == 2);
    absl::string_view container = head_clauses.front();

    rule_parts.front() = head_clauses.back();
    std::vector<ParsedBags>& contained_bags = contains_[container];
    for (absl::string_view contained : rule_parts) {
      if (contained == "no other bags") continue;
      contained_bags.emplace_back(ParseBags(contained));
    }
  }

  int CountHeldBags(absl::string_view bag_color) const {
    auto iter = contains_.find(bag_color);
    if (iter == contains_.end()) return 1;
    int total_bags = 1;
    for (const ParsedBags& held : iter->second) {
      total_bags += held.count * CountHeldBags(held.color);
    }
    return total_bags;
  }

 private:
  struct ParsedBags {
    std::string color;
    int count = 0;
  };

  static ParsedBags ParseBags(absl::string_view bags_str) {
    if (absl::EndsWith(bags_str, " bags")) {
      bags_str.remove_suffix(5);
    } else if (absl::EndsWith(bags_str, " bag")) {
      bags_str.remove_suffix(4);
    } else {
      CHECK_FAIL();
    }

    ParsedBags parsed;
    static re2::LazyRE2 pattern = {"([[:digit:]]+) (.*)"};
    CHECK(
        re2::RE2::FullMatch(bags_str, *pattern, &parsed.count, &parsed.color));
    return parsed;
  }

  absl::flat_hash_map<std::string, std::vector<ParsedBags>> contains_;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);

  BagGraph graph;
  for (const std::string& line : lines) {
    graph.AddRule(line);
  }

  std::cout << (graph.CountHeldBags("shiny gold") - 1) << "\n";

  return 0;
}
