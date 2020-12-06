#include <cctype>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "absl/types/span.h"
#include "re2/re2.h"
#include "util/check.h"
#include "util/io.h"

namespace {

class FieldValidator {
 public:
  std::uint8_t Validate(absl::string_view value) const {
    return IsValid(value) ? check_bit_ : 0;
  }

 protected:
  explicit FieldValidator(int bit_position) : check_bit_(1 << bit_position) {}

 private:
  virtual bool IsValid(absl::string_view value) const = 0;

  std::uint8_t check_bit_;
};

class NoopFieldValidator final : public FieldValidator {
 public:
  explicit NoopFieldValidator(int bit_position)
      : FieldValidator(bit_position) {}

 private:
  bool IsValid(absl::string_view) const override { return true; }
};

class IntFieldValidator final : public FieldValidator {
 public:
  explicit IntFieldValidator(int bit_position, int min, int max)
      : FieldValidator(bit_position), min_(min), max_(max) {}

 private:
  bool IsValid(absl::string_view value) const override {
    int parsed = 0;
    if (!absl::SimpleAtoi(value, &parsed)) return false;
    return (min_ <= parsed) && (parsed <= max_);
  }

  int min_;
  int max_;
};

class RegexFieldValidator final : public FieldValidator {
 public:
  explicit RegexFieldValidator(int bit_position, absl::string_view pattern)
      : FieldValidator(bit_position), pattern_(pattern) {
    CHECK(pattern_.ok());
  }

 private:
  bool IsValid(absl::string_view value) const override {
    return re2::RE2::FullMatch(value, pattern_);
  }

  re2::RE2 pattern_;
};

class HgtFieldValidator final : public FieldValidator {
 public:
  explicit HgtFieldValidator(int bit_position) : FieldValidator(bit_position) {
    CHECK(pattern_.ok());
  }

 private:
  bool IsValid(absl::string_view value) const override {
    int height = 0;
    std::string units;
    if (!re2::RE2::FullMatch(value, pattern_, &height, &units)) return false;
    if (units == "cm") {
      return (150 <= height) && (height <= 193);
    }
    CHECK(units == "in");
    return (59 <= height) && (height <= 76);
  }

  re2::RE2 pattern_{"([[:digit:]]+)(cm|in)"};
};

class PassportFields {
 public:
  explicit PassportFields(absl::Span<const std::string> lines) {
    for (const std::string& line : lines) {
      if (line.empty()) continue;
      for (absl::string_view field_value : absl::StrSplit(line, ' ')) {
        const absl::string_view field = field_value.substr(0, 3);
        CHECK(field_value[3] == ':');
        const absl::string_view value = field_value.substr(4);
        auto iter = validators_->find(field);
        CHECK(iter != validators_->end());
        rep_ |= iter->second->Validate(value);
      }
    }
  }

  bool IsValid() const {
    static const std::uint8_t kMask = ((1 << 7) - 1);
    return (rep_ & kMask) == kMask;
  }

 private:
  static const absl::flat_hash_map<std::string, FieldValidator*>* validators_;

  std::uint8_t rep_ = 0;
};

const absl::flat_hash_map<std::string, FieldValidator*>*
    PassportFields::validators_ =
        new absl::flat_hash_map<std::string, FieldValidator*>{
            {"byr", new IntFieldValidator(0, 1920, 2002)},
            {"iyr", new IntFieldValidator(1, 2010, 2020)},
            {"eyr", new IntFieldValidator(2, 2020, 2030)},
            {"hgt", new HgtFieldValidator(3)},
            {"hcl", new RegexFieldValidator(4, "#[[:xdigit:]]{6}")},
            {"ecl", new RegexFieldValidator(5, "amb|blu|brn|gry|grn|hzl|oth")},
            {"pid", new RegexFieldValidator(6, "[[:digit:]]{9}")},
            {"cid", new NoopFieldValidator(7)}};

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);

  int valid_passports = 0;
  std::size_t min_line = 0;
  for (std::size_t max_line = 0; max_line < lines.size(); ++max_line) {
    if (lines[max_line].empty()) {
      PassportFields fields(
          absl::MakeSpan(lines).subspan(min_line, max_line - min_line));
      valid_passports += fields.IsValid();
      min_line = max_line;
    }
  }

  if (min_line != lines.size() - 1) {
    PassportFields fields(absl::MakeSpan(lines).subspan(min_line));
    valid_passports += fields.IsValid();
  }

  std::cout << valid_passports << "\n";
  return 0;
}
