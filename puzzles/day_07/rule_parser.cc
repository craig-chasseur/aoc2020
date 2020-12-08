#include "puzzles/day_07/rule_parser.h"

#include "absl/strings/string_view.h"
#include "re2/re2.h"
#include "re2/stringpiece.h"
#include "util/check.h"

namespace aoc2020 {
namespace {

absl::string_view ToStringView(re2::StringPiece sp) {
  return absl::string_view(sp.data(), sp.size());
}

}  // namespace

BagRule ParseBagRule(absl::string_view rule_txt) {
  static re2::LazyRE2 kContainerMatcher = {"(.*?) bags contain "};
  static re2::LazyRE2 kContaineeMatcher = {
      "([[:digit:]]+) (.*?) bags?(?:, |.)"};

  re2::StringPiece rule_txt_sp(rule_txt);
  re2::StringPiece container_color;
  CHECK(re2::RE2::Consume(&rule_txt_sp, *kContainerMatcher, &container_color));

  BagRule rule{.container_color = ToStringView(container_color)};
  if (rule_txt_sp == "no other bags.") return rule;

  while (!rule_txt_sp.empty()) {
    int containee_count = 0;
    re2::StringPiece containee_color;
    CHECK(re2::RE2::Consume(&rule_txt_sp, *kContaineeMatcher, &containee_count,
                            &containee_color));
    rule.contained_bags.emplace_back(ContainedBags{
        .color = ToStringView(containee_color), .count = containee_count});
  }
  return rule;
}

}  // namespace aoc2020
