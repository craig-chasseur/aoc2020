#ifndef UTIL_IO_H_
#define UTIL_IO_H_

#include <cstddef>
#include <string>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/numbers.h"

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

}  // namespace aoc2020

#endif  // UTIL_IO_H_
