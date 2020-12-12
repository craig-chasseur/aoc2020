#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "absl/strings/string_view.h"
#include "util/check.h"
#include "util/io.h"

namespace {

enum class Heading : std::uint8_t {
  kEast = 0,
  kSouth = 1,
  kWest = 2,
  kNorth = 3
};

struct PositionAndHeading {
  int ManhattanDistanceFromOrigin() const {
    return std::abs(latitude) + std::abs(longitude);
  }

  int latitude = 0;
  int longitude = 0;
  Heading heading = Heading::kEast;
};

Heading TurnLeft(Heading heading, unsigned degrees) {
  CHECK(degrees % 90 == 0);
  int heading_int = static_cast<int>(heading);
  heading_int -= (degrees / 90);
  while (heading_int < 0) {
    heading_int += 4;
  }
  return static_cast<Heading>(heading_int);
}

Heading TurnRight(Heading heading, unsigned degrees) {
  CHECK(degrees % 90 == 0);
  int heading_int = static_cast<int>(heading);
  heading_int += (degrees / 90);
  heading_int %= 4;
  return static_cast<Heading>(heading_int);
}

PositionAndHeading ApplyStep(PositionAndHeading initial,
                             absl::string_view instruction) {
  char direction;
  unsigned units;
  CHECK(2 == std::sscanf(instruction.data(), "%c%u", &direction, &units));
  switch (direction) {
    case 'N':
      initial.longitude += units;
      return initial;
    case 'S':
      initial.longitude -= units;
      return initial;
    case 'E':
      initial.latitude += units;
      return initial;
    case 'W':
      initial.latitude -= units;
      return initial;
    case 'L':
      initial.heading = TurnLeft(initial.heading, units);
      return initial;
    case 'R':
      initial.heading = TurnRight(initial.heading, units);
      return initial;
    case 'F':
      switch (initial.heading) {
        case Heading::kEast:
          initial.latitude += units;
          return initial;
        case Heading::kSouth:
          initial.longitude -= units;
          return initial;
        case Heading::kWest:
          initial.latitude -= units;
          return initial;
        case Heading::kNorth:
          initial.longitude += units;
          return initial;
      }
    default:
      CHECK_FAIL();
  }
}

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> instructions = aoc2020::ReadLinesFromFile(argv[1]);

  PositionAndHeading pos_heading;
  for (const absl::string_view instruction : instructions) {
    pos_heading = ApplyStep(pos_heading, instruction);
  }
  std::cout << pos_heading.ManhattanDistanceFromOrigin() << "\n";

  return 0;
}
