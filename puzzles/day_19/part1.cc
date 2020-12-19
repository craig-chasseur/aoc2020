#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/memory/memory.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "util/check.h"
#include "util/io.h"

namespace {

class Rule {
 public:
  virtual ~Rule() = default;

  virtual void MatchAndConsume(
      std::vector<absl::string_view>* candidate_strs,
      const absl::flat_hash_map<int, std::unique_ptr<Rule>>& rule_map)
      const = 0;

  bool Match(
      absl::string_view str,
      const absl::flat_hash_map<int, std::unique_ptr<Rule>>& rule_map) const {
    std::vector<absl::string_view> candidate_strs{str};
    MatchAndConsume(&candidate_strs, rule_map);
    if (candidate_strs.empty()) return false;
    for (const absl::string_view str : candidate_strs) {
      if (str.empty()) return true;
    }
    return false;
  }
};

class LiteralRule final : public Rule {
 public:
  explicit LiteralRule(char c) : c_(c) {}

  explicit LiteralRule(absl::string_view rule_txt) {
    CHECK(rule_txt.size() == 3);
    c_ = rule_txt[1];
  }

  ~LiteralRule() override = default;

  void MatchAndConsume(std::vector<absl::string_view>* candidate_strs,
                       const absl::flat_hash_map<int, std::unique_ptr<Rule>>&
                           rule_map) const override {
    auto end = std::remove_if(candidate_strs->begin(), candidate_strs->end(),
                              [this](absl::string_view str) {
                                return str.empty() || str.front() != c_;
                              });
    candidate_strs->erase(end, candidate_strs->end());
    for (absl::string_view& str : *candidate_strs) {
      str.remove_prefix(1);
    }
  }

 private:
  char c_;
};

class ConcatenationRule final : public Rule {
 public:
  explicit ConcatenationRule(std::vector<int> subrules)
      : subrules_(std::move(subrules)) {}

  explicit ConcatenationRule(absl::string_view rule_txt) {
    for (absl::string_view rule_idx_txt : absl::StrSplit(rule_txt, ' ')) {
      int rule_idx = 0;
      CHECK(absl::SimpleAtoi(rule_idx_txt, &rule_idx));
      subrules_.push_back(rule_idx);
    }
  }

  ~ConcatenationRule() override = default;

  void MatchAndConsume(std::vector<absl::string_view>* candidate_strs,
                       const absl::flat_hash_map<int, std::unique_ptr<Rule>>&
                           rule_map) const override {
    for (const int subrule_idx : subrules_) {
      const auto rule_iter = rule_map.find(subrule_idx);
      CHECK(rule_iter != rule_map.end());
      rule_iter->second->MatchAndConsume(candidate_strs, rule_map);
    }
  }

 private:
  std::vector<int> subrules_;
};

class AlternationRule final : public Rule {
 public:
  explicit AlternationRule(std::vector<ConcatenationRule> children)
      : children_(std::move(children)) {}

  explicit AlternationRule(absl::string_view rule_txt) {
    for (absl::string_view subrule_txt : absl::StrSplit(rule_txt, " | ")) {
      children_.emplace_back(subrule_txt);
    }
  }

  ~AlternationRule() override = default;

  void MatchAndConsume(std::vector<absl::string_view>* candidate_strs,
                       const absl::flat_hash_map<int, std::unique_ptr<Rule>>&
                           rule_map) const override {
    std::vector<absl::string_view> candidate_strs_merged;
    for (const ConcatenationRule& child : children_) {
      std::vector<absl::string_view> candidate_strs_copy(*candidate_strs);
      child.MatchAndConsume(&candidate_strs_copy, rule_map);
      candidate_strs_merged.insert(candidate_strs_merged.end(),
                                   candidate_strs_copy.begin(),
                                   candidate_strs_copy.end());
    }
    *candidate_strs = std::move(candidate_strs_merged);
  }

 private:
  std::vector<ConcatenationRule> children_;
};

absl::flat_hash_map<int, std::unique_ptr<Rule>> ParseRules(
    const std::vector<std::string>& rule_lines) {
  absl::flat_hash_map<int, std::unique_ptr<Rule>> rule_map;
  for (const absl::string_view rule_line : rule_lines) {
    std::vector<absl::string_view> parts = absl::StrSplit(rule_line, ": ");
    CHECK(parts.size() == 2);
    int rule_idx = 0;
    CHECK(absl::SimpleAtoi(parts.front(), &rule_idx));

    std::unique_ptr<Rule> rule;
    if (parts.back().find('"') != absl::string_view::npos) {
      rule = absl::make_unique<LiteralRule>(parts.back());
    } else if (parts.back().find('|') != absl::string_view::npos) {
      rule = absl::make_unique<AlternationRule>(parts.back());
    } else {
      rule = absl::make_unique<ConcatenationRule>(parts.back());
    }

    CHECK(rule_map.try_emplace(rule_idx, std::move(rule)).second);
  }
  return rule_map;
}

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);
  std::vector<std::vector<std::string>> line_groups =
      aoc2020::SplitByEmptyStrings(std::move(lines));
  CHECK(line_groups.size() == 2);

  absl::flat_hash_map<int, std::unique_ptr<Rule>> rule_map =
      ParseRules(line_groups.front());
  auto rule0_iter = rule_map.find(0);
  CHECK(rule0_iter != rule_map.end());

  int matched = 0;
  for (absl::string_view str : line_groups.back()) {
    matched += rule0_iter->second->Match(str, rule_map);
  }
  std::cout << matched << "\n";

  return 0;
}
