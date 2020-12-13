#include <__bit_reference>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "util/check.h"
#include "util/io.h"

namespace {

struct BusConstraints {
  std::int64_t id = 0;
  std::int64_t offset = 0;
};

std::int64_t GcdExtended(std::int64_t a, std::int64_t b, std::int64_t* x,
                         std::int64_t* y) {
  if (a == 0) {
    *x = 0;
    *y = 1;
    return b;
  }

  std::int64_t x1 = 0;
  std::int64_t y1 = 0;
  const std::int64_t gcd = GcdExtended(b % a, a, &x1, &y1);

  *x = y1 - (b / a) * x1;
  *y = x1;
  return gcd;
}

std::int64_t ModInverse(std::int64_t a, std::int64_t m) {
  std::int64_t x = 0;
  std::int64_t y = 0;
  std::int64_t g = GcdExtended(a, m, &x, &y);
  return (x % m + m) % m;
}

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);

  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);
  CHECK(lines.size() == 2);

  std::vector<BusConstraints> bus_constraints;
  std::int64_t offset = 0;
  for (absl::string_view bus_id_str : absl::StrSplit(lines.back(), ',')) {
    std::int64_t bus_id_parsed = 0;
    if (absl::SimpleAtoi(bus_id_str, &bus_id_parsed)) {
      bus_constraints.push_back(
          BusConstraints{.id = bus_id_parsed, .offset = offset});
    } else {
      CHECK(bus_id_str == "x");
    }
    ++offset;
  }

  std::int64_t product = 1;
  for (const BusConstraints& bus : bus_constraints) {
    product *= bus.id;
  }

  std::int64_t result = 0;
  for (const BusConstraints& bus : bus_constraints) {
    std::int64_t partial_product = product / bus.id;
    result +=
        bus.offset * ModInverse(partial_product, bus.id) * partial_product;
  }
  result %= product;

  std::cout << (product - result) << "\n";

  return 0;
}
