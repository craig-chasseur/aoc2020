#include <cstdint>
#include <string>
#include <vector>

#include "util/check.h"
#include "util/io.h"

namespace {

std::int64_t DetermineLoopSize(const std::int64_t subject,
                               const std::int64_t result) {
  std::int64_t value = 1;
  std::int64_t loops = 0;
  while (value != result) {
    ++loops;
    value *= subject;
    value %= 20201227;
  }
  return loops;
}

std::int64_t ApplyLoops(const std::int64_t subject, const std::int64_t loops) {
  std::int64_t value = 1;
  for (std::int64_t i = 0; i < loops; ++i) {
    value *= subject;
    value %= 20201227;
  }
  return value;
}

}  // namespace

int main(int argc, char** argv) {
  CHECK(2 == argc);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);
  CHECK(2 == lines.size());
  std::vector<std::int64_t> public_keys =
      aoc2020::ParseIntegers<std::int64_t>(lines);
  CHECK(2 == public_keys.size());

  const std::int64_t card_loop_size = DetermineLoopSize(7, public_keys.front());
  const std::int64_t door_loop_size = DetermineLoopSize(7, public_keys.back());

  const std::int64_t encryption_key =
      ApplyLoops(public_keys.back(), card_loop_size);
  CHECK(encryption_key == ApplyLoops(public_keys.front(), door_loop_size));
  std::cout << encryption_key << "\n";

  return 0;
}
