#ifndef PUZZLES_DAY_07_RULE_PARSER_H_
#define PUZZLES_DAY_07_RULE_PARSER_H_

#include <vector>

#include "absl/strings/string_view.h"

namespace aoc2020 {

struct ContainedBags {
  absl::string_view color;
  int count = 0;
};

struct BagRule {
  absl::string_view container_color;
  std::vector<ContainedBags> contained_bags;
};

BagRule ParseBagRule(absl::string_view rule_txt);

}  // namespace aoc2020

#endif  // PUZZLES_DAY_07_RULE_PARSER_H_
