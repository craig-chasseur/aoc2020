#include "util/io.h"

#include <fstream>
#include <ios>
#include <string>
#include <utility>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"

namespace aoc2020 {

absl::StatusOr<std::string> ReadFile(const char* filename) {
  std::ifstream stream(filename);
  if (!stream) {
    return absl::Status(absl::StatusCode::kNotFound,
                        absl::StrCat("Unable to open file: ", filename));
  }

  std::string buffer;
  stream.seekg(0, std::ios::end);
  buffer.resize(stream.tellg());
  stream.seekg(0, std::ios::beg);
  stream.read(&buffer[0], buffer.size());
  stream.close();

  return buffer;
}

absl::StatusOr<std::vector<std::string>> ReadLinesFromFile(
    const char* filename) {
  std::ifstream stream(filename);
  if (!stream) {
    return absl::Status(absl::StatusCode::kNotFound,
                        absl::StrCat("Unable to open file: ", filename));
  }

  std::vector<std::string> all_lines;
  std::string line;
  while (std::getline(stream, line)) {
    all_lines.emplace_back(std::move(line));
  }
  stream.close();
  return all_lines;
}

absl::StatusOr<std::vector<std::string>> ReadCommaDelimitedFile(
    const char* filename) {
  absl::StatusOr<std::string> maybe_contents = ReadFile(filename);
  if (!maybe_contents.ok()) return maybe_contents.status();
  return absl::StrSplit(*maybe_contents, ',');
}

}  // namespace aoc2020
