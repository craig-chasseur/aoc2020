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

struct Coords {
  int ManhattanDistanceFromOrigin() const {
    return std::abs(ship_latitude) + std::abs(ship_longitude);
  }

  int ship_latitude = 0;
  int ship_longitude = 0;
  int waypoint_latitude = 10;
  int waypoint_longitude = 1;
};

Coords TurnLeft90(Coords coords) {
  int tmp_latitude = -coords.waypoint_longitude;
  coords.waypoint_longitude = coords.waypoint_latitude;
  coords.waypoint_latitude = tmp_latitude;
  return coords;
}

Coords TurnRight90(Coords coords) {
  int tmp_latitude = coords.waypoint_longitude;
  coords.waypoint_longitude = -coords.waypoint_latitude;
  coords.waypoint_latitude = tmp_latitude;
  return coords;
}

Coords ApplyStep(Coords initial, absl::string_view instruction) {
  char direction;
  unsigned units;
  CHECK(2 == std::sscanf(instruction.data(), "%c%u", &direction, &units));
  switch (direction) {
    case 'N':
      initial.waypoint_longitude += units;
      return initial;
    case 'S':
      initial.waypoint_longitude -= units;
      return initial;
    case 'E':
      initial.waypoint_latitude += units;
      return initial;
    case 'W':
      initial.waypoint_latitude -= units;
      return initial;
    case 'L': {
      CHECK(units % 90 == 0);
      for (int i = 0; i < ((units / 90) % 4); ++i) {
        initial = TurnLeft90(initial);
      }
      return initial;
    }
    case 'R': {
      CHECK(units % 90 == 0);
      for (int i = 0; i < ((units /90) % 4); ++i) {
        initial = TurnRight90(initial);
      }
      return initial;
    }
    case 'F':
      initial.ship_latitude += units * initial.waypoint_latitude;
      initial.ship_longitude += units * initial.waypoint_longitude;
      return initial;
    default:
      CHECK_FAIL();
  }
}

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> instructions = aoc2020::ReadLinesFromFile(argv[1]);

  Coords coords;
  for (const absl::string_view instruction : instructions) {
    coords = ApplyStep(coords, instruction);
  }
  std::cout << coords.ManhattanDistanceFromOrigin() << "\n";

  return 0;
}
