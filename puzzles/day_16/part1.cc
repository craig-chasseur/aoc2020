#include <iostream>
#include <string>
#include <vector>

#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "absl/types/span.h"
#include "re2/re2.h"
#include "util/check.h"
#include "util/io.h"

namespace {

class Rule {
 public:
  explicit Rule(absl::string_view rule_line) {
    static re2::LazyRE2 kPattern = {
        "(.*?): ([[:digit:]]+)-([[:digit:]]+) or "
        "([[:digit:]]+)-([[:digit:]]+)"};
    CHECK(re2::RE2::FullMatch(rule_line, *kPattern, &field_name_, &min_a_,
                              &max_a_, &min_b_, &max_b_));
  }

  bool CheckValue(int value) const {
    return (value >= min_a_ && value <= max_a_) ||
           (value >= min_b_ && value <= max_b_);
  }

 private:
  std::string field_name_;
  int min_a_ = 0;
  int max_a_ = 0;
  int min_b_ = 0;
  int max_b_ = 0;
};

class Ticket {
 public:
  explicit Ticket(absl::string_view ticket_line) {
    for (const absl::string_view field_str : absl::StrSplit(ticket_line, ',')) {
      int value = 0;
      CHECK(absl::SimpleAtoi(field_str, &value));
      field_values_.push_back(value);
    }
  }

  int ErrorRate(absl::Span<const Rule> rules) const {
    int error_rate = 0;
    for (const int value : field_values_) {
      bool valid = false;
      for (const Rule& rule : rules) {
        if (rule.CheckValue(value)) {
          valid = true;
          break;
        }
      }
      if (!valid) error_rate += value;
    }
    return error_rate;
  }

 private:
  std::vector<int> field_values_;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK(2 == argc);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);
  std::vector<std::vector<std::string>> line_groups =
      aoc2020::SplitByEmptyStrings(std::move(lines));
  CHECK(3 == line_groups.size());

  std::vector<Rule> rules;
  for (const absl::string_view rule_line : line_groups[0]) {
    rules.emplace_back(rule_line);
  }

  CHECK(line_groups.back().front() == "nearby tickets:");
  std::vector<Ticket> tickets;
  for (auto ticket_line_iter = line_groups.back().begin() + 1;
       ticket_line_iter != line_groups.back().end(); ++ticket_line_iter) {
    tickets.emplace_back(*ticket_line_iter);
  }

  int error_rate = 0;
  for (const Ticket& ticket : tickets) {
    error_rate += ticket.ErrorRate(rules);
  }
  std::cout << error_rate << "\n";

  return 0;
}
