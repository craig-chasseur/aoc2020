#ifndef UTIL_IO_H_
#define UTIL_IO_H_

#include <cstddef>
#include <string>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/numbers.h"
#include "absl/strings/string_view.h"

namespace aoc2020 {

// Returns the whole contents of the text file specified by `filename`, or an
// error if the file could not be read.
absl::StatusOr<std::string> ReadFile(const char* filename);

// Returns each of the lines in the text file specified by `filename`, or an
// error if the file could not be read.
absl::StatusOr<std::vector<std::string>> ReadLinesFromFile(
    const char* filename);

// Returns comma-delimited strings from `filename`, or an error if the file
// could not be read.
absl::StatusOr<std::vector<std::string>> ReadCommaDelimitedFile(
    const char* filename);

template <typename IntType = int>
absl::StatusOr<std::vector<IntType>> ParseIntegers(
    const std::vector<std::string>& strings) {
  std::vector<IntType> integers(strings.size(), 0);
  for (std::size_t idx = 0; idx < strings.size(); ++idx) {
    if (!absl::SimpleAtoi(strings[idx], &(integers[idx]))) {
      return absl::Status(absl::StatusCode::kInvalidArgument,
                          "Failed to parse int from string");
    }
  }
  return integers;
}

#define SUBSTRING_FMT "%zn%*s%zn"

// A helper for use with sscanf to read a substring as an absl::string_view
// without copying underlying data. To use, put SUBSTRING_FMT into the format
// string and supply the two parameters FirstParam() and SecondParam() in the
// corresponding positions in output parameters. Note that a successful match
// does not increment the match count at all. Finally call Result() to get the
// extracted substring. Example:
//
//   SubstringScanHelper helper(some_string);
//   const int matches = sscanf(some_string, "foo " SUBSTRING_FMT " bar",
//                              helper.FirstParam(), helper.SecondParam());
//   assert(matches == 0);
//   absl::string_view substr = helper.Result();
//
class SubstringScanHelper {
 public:
  static inline constexpr char kFormat[] = SUBSTRING_FMT;

  explicit SubstringScanHelper(absl::string_view original_str)
      : original_str_(original_str) {}

  std::size_t* FirstParam() { return &min_idx_; }
  std::size_t* SecondParam() { return &max_idx_; };

  absl::string_view Result() const {
    return original_str_.substr(min_idx_, max_idx_ - min_idx_);
  }

 private:
  absl::string_view original_str_;
  std::size_t min_idx_ = 0;
  std::size_t max_idx_ = 0;
};

}  // namespace aoc2020

#endif  // UTIL_IO_H_
