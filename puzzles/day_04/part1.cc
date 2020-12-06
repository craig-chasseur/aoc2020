#include <cstddef>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
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
        auto iter = fields_->find(field_value.substr(0, 3));
        CHECK(iter != fields_->end());
        rep_ |= iter->second;
      }
    }
  }

  bool IsValid() const {
    static const std::uint8_t kMask = ((1 << 7) - 1);
    return (rep_ & kMask) == kMask;
  }

 private:
  static const absl::flat_hash_map<std::string, std::uint8_t>* fields_;

  std::uint8_t rep_ = 0;
};

const absl::flat_hash_map<std::string, std::uint8_t>* PassportFields::fields_ =
    new absl::flat_hash_map<std::string, std::uint8_t>{
        {"byr", 1 << 0}, {"iyr", 1 << 1}, {"eyr", 1 << 2}, {"hgt", 1 << 3},
        {"hcl", 1 << 4}, {"ecl", 1 << 5}, {"pid", 1 << 6}, {"cid", 1 << 7}};

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);
  std::vector<std::vector<std::string>> passport_strs =
      aoc2020::SplitByEmptyStrings(std::move(lines));

  int valid_passports = 0;
  for (const std::vector<std::string> passport_desc : passport_strs) {
    valid_passports += PassportFields(passport_desc).IsValid();
  }
  std::cout << valid_passports << "\n";

  return 0;
}
