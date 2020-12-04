#include <cctype>
#include <cstddef>
#include <cstdint>
#include <iostream>

#include "absl/container/flat_hash_set.h"
#include "absl/strings/ascii.h"
#include "absl/strings/match.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "absl/types/optional.h"
#include "absl/types/span.h"
#include "util/check.h"
#include "util/io.h"

namespace {

class PassportFields {
 public:
  explicit PassportFields(absl::Span<const std::string> lines) {
    for (const std::string& line : lines) {
      if (line.empty()) continue;
      for (absl::string_view field_value : absl::StrSplit(line, ' ')) {
        const absl::string_view field = field_value.substr(0, 3);
        CHECK(field_value[3] == ':');
        const absl::string_view value = field_value.substr(4);
        ValidateField(field, value);
      }
    }
  }

  bool IsValid() const {
    static const std::uint8_t kMask = ((1 << 7) - 1);
    return (rep_ & kMask) == kMask;
  }

 private:
  void ValidateField(absl::string_view field, absl::string_view value) {
    if (field == "byr") {
      if (ValidByr(value)) {
        rep_ |= (1 << 0);
      }
      return;
    }

    if (field == "iyr") {
      if (ValidIyr(value)) {
        rep_ |= (1 << 1);
      }
      return;
    }

    if (field == "eyr") {
      if (ValidEyr(value)) {
        rep_ |= (1 << 2);
      }
      return;
    }

    if (field == "hgt") {
      if (ValidHgt(value)) {
        rep_ |= (1 << 3);
      }
      return;
    }

    if (field == "hcl") {
      if (ValidHcl(value)) {
        rep_ |= (1 << 4);
      }
      return;
    }

    if (field == "ecl") {
      if (ValidEcl(value)) {
        rep_ |= (1 << 5);
      }
      return;
    }

    if (field == "pid") {
      if (ValidPid(value)) {
        rep_ |= (1 << 6);
      }
      return;
    }

    if (field == "cid") return;
    CHECK_FAIL();
  }

  static absl::optional<int> ParseInt(absl::string_view year_str) {
    int parsed = 0;
    if (!absl::SimpleAtoi(year_str, &parsed)) return absl::nullopt;
    return parsed;
  }

  static bool ValidByr(absl::string_view value) {
    absl::optional<int> year = ParseInt(value);
    return year.has_value() && (*year >= 1920) && (*year <= 2002);
  }

  static bool ValidIyr(absl::string_view value) {
    absl::optional<int> year = ParseInt(value);
    return year.has_value() && (*year >= 2010) && (*year <= 2020);
  }

  static bool ValidEyr(absl::string_view value) {
    absl::optional<int> year = ParseInt(value);
    return year.has_value() && (*year >= 2020) && (*year <= 2030);
  }

  static bool ValidHgt(absl::string_view value) {
    if (absl::EndsWith(value, "cm")) {
      value.remove_suffix(2);
      absl::optional<int> height_cm = ParseInt(value);
      return height_cm.has_value() && (*height_cm >= 150) &&
             (*height_cm <= 193);
    }

    if (absl::EndsWith(value, "in")) {
      value.remove_suffix(2);
      absl::optional<int> height_in = ParseInt(value);
      return height_in.has_value() && (*height_in >= 59) && (*height_in <= 76);
    }

    return false;
  }

  static bool ValidHcl(absl::string_view value) {
    if (value.size() != 7) return false;
    if (value[0] != '#') return false;
    for (char c : value.substr(1)) {
      if (!absl::ascii_isxdigit(c)) return false;
    }
    return true;
  }

  static bool ValidEcl(absl::string_view value) {
    return valid_eyecolors_->contains(value);
  }

  static bool ValidPid(absl::string_view value) {
    if (value.size() != 9) return false;
    for (char c : value) {
      if (!std::isdigit(c)) return false;
    }
    return true;
  }

  static const absl::flat_hash_set<std::string>* valid_eyecolors_;

  std::uint8_t rep_ = 0;
};

const absl::flat_hash_set<std::string>* PassportFields::valid_eyecolors_ =
    new absl::flat_hash_set<std::string>{"amb", "blu", "brn", "gry",
                                         "grn", "hzl", "oth"};

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);
  auto maybe_lines = aoc2020::ReadLinesFromFile(argv[1]);
  CHECK_OK(maybe_lines);

  int valid_passports = 0;
  std::size_t min_line = 0;
  for (std::size_t max_line = 0; max_line < maybe_lines->size(); ++max_line) {
    if ((*maybe_lines)[max_line].empty()) {
      PassportFields fields(
          absl::MakeSpan(*maybe_lines).subspan(min_line, max_line - min_line));
      valid_passports += fields.IsValid();
      min_line = max_line;
    }
  }

  if (min_line != maybe_lines->size() - 1) {
    PassportFields fields(absl::MakeSpan(*maybe_lines).subspan(min_line));
    valid_passports += fields.IsValid();
  }

  std::cout << valid_passports << "\n";
  return 0;
}
