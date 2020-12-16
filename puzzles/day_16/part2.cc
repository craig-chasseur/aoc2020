#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/match.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "absl/types/span.h"
#include "re2/re2.h"
#include "util/check.h"
#include "util/io.h"

namespace {

class Ticket;

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

  bool ValidInPosition(int position, absl::Span<const Ticket> tickets) const;

  void ComputeValidPositions(const int num_positions,
                             absl::Span<const Ticket> tickets);

  absl::string_view field_name() const { return field_name_; }

  absl::Span<const int> valid_positions() const {
    CHECK(!valid_positions_.empty());
    return valid_positions_;
  }

 private:
  std::string field_name_;
  int min_a_ = 0;
  int max_a_ = 0;
  int min_b_ = 0;
  int max_b_ = 0;
  std::vector<int> valid_positions_;
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

  bool IsValid(absl::Span<const Rule> rules) const {
    for (const int value : field_values_) {
      bool valid = false;
      for (const Rule& rule : rules) {
        if (rule.CheckValue(value)) {
          valid = true;
          break;
        }
      }
      if (!valid) return false;
    }
    return true;
  }

  int ValueNum(int num) const {
    CHECK(num < field_values_.size());
    return field_values_[num];
  }

 private:
  std::vector<int> field_values_;
};

bool Rule::ValidInPosition(int position,
                           absl::Span<const Ticket> tickets) const {
  for (const Ticket& ticket : tickets) {
    const int value = ticket.ValueNum(position);
    const bool valid = (min_a_ <= value && value <= max_a_) ||
                       (min_b_ <= value && value <= max_b_);
    if (!valid) return false;
  }

  return true;
}

void Rule::ComputeValidPositions(const int num_positions,
                                 absl::Span<const Ticket> tickets) {
  CHECK(valid_positions_.empty());
  for (int pos = 0; pos < num_positions; ++pos) {
    if (ValidInPosition(pos, tickets)) valid_positions_.push_back(pos);
  }
  CHECK(!valid_positions_.empty());
}

bool FindOrderImpl(absl::Span<const Rule> remaining_rules,
                   absl::flat_hash_map<int, const Rule*>* rule_assignments) {
  if (remaining_rules.empty()) return true;

  const Rule& current_rule = remaining_rules.front();
  for (const int candidate_position : current_rule.valid_positions()) {
    auto [_, inserted] =
        rule_assignments->try_emplace(candidate_position, &current_rule);
    if (!inserted) continue;
    if (FindOrderImpl(remaining_rules.subspan(1), rule_assignments)) {
      return true;
    }
    CHECK(1 == rule_assignments->erase(candidate_position));
  }

  return false;
}

std::vector<Rule> FindOrder(absl::Span<const Rule> rules) {
  absl::flat_hash_map<int, const Rule*> rule_assignments;
  CHECK(FindOrderImpl(rules, &rule_assignments));
  CHECK(rule_assignments.size() == rules.size());

  std::vector<Rule> ordered_rules;
  for (int i = 0; i < rules.size(); ++i) {
    auto iter = rule_assignments.find(i);
    CHECK(iter != rule_assignments.end());
    ordered_rules.emplace_back(*iter->second);
  }
  return ordered_rules;
}

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

  CHECK(line_groups[1].front() == "your ticket:");
  Ticket my_ticket(line_groups[1].back());

  CHECK(line_groups.back().front() == "nearby tickets:");
  std::vector<Ticket> tickets;
  for (auto ticket_line_iter = line_groups.back().begin() + 1;
       ticket_line_iter != line_groups.back().end(); ++ticket_line_iter) {
    tickets.emplace_back(*ticket_line_iter);
  }

  std::vector<Ticket> valid_tickets;
  valid_tickets.push_back(my_ticket);
  for (Ticket& ticket : tickets) {
    if (ticket.IsValid(rules)) {
      valid_tickets.emplace_back(std::move(ticket));
    }
  }

  for (Rule& rule : rules) {
    rule.ComputeValidPositions(rules.size(), valid_tickets);
  }

  // The secret sauce... try the most-constrained rules first in the depth-first
  // search and the fanout gets way smaller.
  std::sort(rules.begin(), rules.end(), [](const Rule& a, const Rule& b) {
    return a.valid_positions().size() < b.valid_positions().size();
  });

  std::vector<Rule> ordered_rules = FindOrder(rules);
  std::int64_t product = 1;
  for (int rule_pos = 0; rule_pos < ordered_rules.size(); ++rule_pos) {
    if (absl::StartsWith(ordered_rules[rule_pos].field_name(), "departure")) {
      product *= static_cast<std::int64_t>(my_ticket.ValueNum(rule_pos));
    }
  }
  std::cout << product << "\n";

  return 0;
}
