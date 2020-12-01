#ifndef UTIL_IO_H_
#define UTIL_IO_H_

#include <string>
#include <vector>

#include "absl/status/statusor.h"

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

}  // namespace aoc2020

#endif  // UTIL_IO_H_
