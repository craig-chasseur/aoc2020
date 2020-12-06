#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

#include "absl/strings/string_view.h"
#include "util/check.h"
#include "util/io.h"

namespace {

struct Password {
  bool IsValid() const {
    const bool min_match = password[min - 1] == validated_char;
    const bool max_match = password[max - 1] == validated_char;
    return min_match ^ max_match;
  }

  char validated_char;
  unsigned min;
  unsigned max;
  std::string password;
};

Password ParsePassword(absl::string_view password_str) {
  Password result;
  aoc2020::SubstringScanHelper pw_helper(password_str);
  CHECK(3 == sscanf(password_str.data(), "%u-%u %c: " SUBSTRING_FMT,
                    &result.min, &result.max, &result.validated_char,
                    pw_helper.FirstParam(), pw_helper.SecondParam()));
  result.password = pw_helper.Result();
  return result;
}

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);

  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);

  int num_valid = 0;
  for (const std::string& password_line : lines) {
    num_valid += ParsePassword(password_line).IsValid();
  }
  std::cout << num_valid << "\n";

  return 0;
}
