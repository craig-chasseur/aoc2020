#include <algorithm>
#include <cstdio>
#include <string>

#include "absl/memory/memory.h"
#include "absl/strings/string_view.h"
#include "util/check.h"
#include "util/io.h"

namespace {

struct Password {
  bool IsValid() const {
    const unsigned occurences =
        std::count(password.begin(), password.end(), validated_char);
    return (min <= occurences) && (occurences <= max);
  }

  char validated_char;
  unsigned min;
  unsigned max;
  std::string password;
};

Password ParsePassword(absl::string_view password_str) {
  auto password_buffer = absl::make_unique<char[]>(password_str.length() + 1);
  Password result;
  CHECK(4 == sscanf(password_str.data(), "%u-%u %c: %s", &result.min,
                    &result.max, &result.validated_char,
                    password_buffer.get()));
  result.password = password_buffer.get();
  return result;
}

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);

  auto maybe_lines = aoc2020::ReadLinesFromFile(argv[1]);
  CHECK_OK(maybe_lines);

  int num_valid = 0;
  for (const std::string& password_line : *maybe_lines) {
    num_valid += ParsePassword(password_line).IsValid();
  }
  std::cout << num_valid << "\n";

  return 0;
}
