#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "puzzles/day_20/bit_square.h"
#include "util/check.h"
#include "util/io.h"

using aoc2020::jigsaw::BitSquare;

namespace {

void DumpBitSquare(const BitSquare& bs) {
  for (int y = 0; y < bs.dim(); ++y) {
    for (int x = 0; x < bs.dim(); ++x) {
      std::cout << (bs.Get(y, x) ? '#' : '.');
    }
    std::cout << "\n";
  }
}

class Tile {
 public:
  explicit Tile(const std::vector<std::string>& line_group) : rep_(10) {
    CHECK(line_group.size() == 11);
    CHECK(1 == std::sscanf(line_group.front().c_str(), "Tile %d:", &id_));
    for (int row_idx = 0; row_idx < 10; ++row_idx) {
      for (int col_idx = 0; col_idx < 10; ++col_idx) {
        rep_.Set(row_idx, col_idx, line_group[row_idx + 1][col_idx] == '#');
      }
    }
  }

  int id() const { return id_; }

  std::vector<std::uint16_t> GetEdgeStrips() const {
    return {rep_.SliceHorizontal(0),
            rep_.SliceHorizontal(9),
            rep_.SliceVertical(0),
            rep_.SliceVertical(9),
            rep_.FlipHorizonal().SliceHorizontal(0),
            rep_.FlipHorizonal().SliceHorizontal(9),
            rep_.FlipVertical().SliceVertical(0),
            rep_.FlipVertical().SliceVertical(9)};
  }

  std::vector<BitSquare> AllOrientations() const {
    return rep_.AllOrientations();
  }

 private:
  int id_ = 0;
  BitSquare rep_;
};

bool FitsHorizontal(const BitSquare& left, const BitSquare& right) {
  CHECK(left.dim() == right.dim());
  return left.SliceVertical(left.dim() - 1) == right.SliceVertical(0);
}

bool FitsVertical(const BitSquare& above, const BitSquare& below) {
  CHECK(above.dim() == below.dim());
  return above.SliceHorizontal(above.dim() - 1) == below.SliceHorizontal(0);
}

bool FitsUpAndLeft(const std::vector<std::vector<BitSquare>>& grid,
                   const BitSquare& candidate, int y, int x) {
  if (y > 0 && !FitsVertical(grid[y - 1][x], candidate)) return false;
  if (x > 0 && !FitsHorizontal(grid[y][x - 1], candidate)) return false;
  return true;
}

struct CategorizedTiles {
  std::vector<Tile> corners;
  std::vector<Tile> edges;
  std::vector<Tile> inner;
};

CategorizedTiles CategorizeTiles(std::vector<Tile> tiles) {
  const int dim = std::sqrt(tiles.size());
  CHECK(dim * dim == tiles.size());

  absl::flat_hash_map<std::uint16_t, int> edge_counts;
  for (const Tile& tile : tiles) {
    for (std::uint16_t edge : tile.GetEdgeStrips()) {
      ++edge_counts[edge];
    }
  }

  CategorizedTiles categorized_tiles;
  for (Tile& tile : tiles) {
    int num_unique = 0;
    for (std::uint16_t unique_edge : tile.GetEdgeStrips()) {
      if (edge_counts[unique_edge] == 1) ++num_unique;
    }
    switch (num_unique) {
      case 0:
        categorized_tiles.inner.emplace_back(std::move(tile));
        break;
      case 2:
        categorized_tiles.edges.emplace_back(std::move(tile));
        break;
      case 4:
        categorized_tiles.corners.emplace_back(std::move(tile));
        break;
      default:
        CHECK_FAIL();
    }
  }
  CHECK(categorized_tiles.corners.size() == 4);
  CHECK(categorized_tiles.edges.size() == (dim - 2) * 4);
  CHECK(categorized_tiles.inner.size() == (dim - 2) * (dim - 2));

  return categorized_tiles;
}

bool SolveImpl(const CategorizedTiles& categorized_tiles, int y, int x,
               std::vector<std::vector<BitSquare>>* solution,
               absl::flat_hash_set<int>* assigned_tile_ids) {
  const int dim = solution->size();
  if (y == dim) {
    CHECK(assigned_tile_ids->size() == dim * dim);
    return true;
  }

  const std::vector<Tile>* candidate_tiles = nullptr;
  const bool on_vertical_edge = x == 0 || x == dim - 1;
  const bool on_horizontal_edge = y == 0 || y == dim - 1;
  if (on_vertical_edge && on_horizontal_edge) {
    candidate_tiles = &categorized_tiles.corners;
  } else if (on_vertical_edge || on_horizontal_edge) {
    candidate_tiles = &categorized_tiles.edges;
  } else {
    candidate_tiles = &categorized_tiles.inner;
  }

  int next_y, next_x;
  if (x == dim - 1) {
    next_y = y + 1;
    next_x = 0;
  } else {
    next_y = y;
    next_x = x + 1;
  }

  for (const Tile& tile : *candidate_tiles) {
    if (assigned_tile_ids->contains(tile.id())) continue;
    for (BitSquare& orientation : tile.AllOrientations()) {
      if (!FitsUpAndLeft(*solution, orientation, y, x)) continue;
      (*solution)[y][x] = std::move(orientation);
      CHECK(assigned_tile_ids->insert(tile.id()).second);
      if (SolveImpl(categorized_tiles, next_y, next_x, solution,
                    assigned_tile_ids)) {
        return true;
      }
      CHECK(1 == assigned_tile_ids->erase(tile.id()));
    }
  }

  return false;
}

std::vector<std::vector<BitSquare>> SolvePuzzle(std::vector<Tile> tiles) {
  const int dim = std::sqrt(tiles.size());
  CHECK(dim * dim == tiles.size());

  const CategorizedTiles categorized_tiles = CategorizeTiles(std::move(tiles));
  std::vector<std::vector<BitSquare>> solution(dim,
                                               std::vector<BitSquare>(dim));
  absl::flat_hash_set<int> assigned_tile_ids;
  CHECK(SolveImpl(categorized_tiles, 0, 0, &solution, &assigned_tile_ids));

  return solution;
}

BitSquare Assemble(const std::vector<std::vector<BitSquare>>& elements) {
  const int outer_dim = elements.size();
  CHECK(outer_dim > 0);
  const int inner_dim = elements.front().front().dim();
  CHECK(inner_dim > 2);
  const int assembled_dim = outer_dim * (inner_dim - 2);

  BitSquare assembled(assembled_dim);
  for (int y = 0; y < assembled_dim; ++y) {
    const int orig_outer_y = y / (inner_dim - 2);
    const int orig_inner_y = y - (orig_outer_y * (inner_dim - 2)) + 1;
    for (int x = 0; x < assembled_dim; ++x) {
      const int orig_outer_x = x / (inner_dim - 2);
      const int orig_inner_x = x - (orig_outer_x * (inner_dim - 2)) + 1;
      assembled.Set(
          y, x,
          elements[orig_outer_y][orig_outer_x].Get(orig_inner_y, orig_inner_x));
    }
  }

  return assembled;
}

struct Offsets {
  std::uint8_t y = 0;
  std::uint8_t x = 0;
};

constexpr Offsets kSeaMonster[] = {{0, 18}, {1, 0},  {1, 5},  {1, 6},  {1, 11},
                                   {1, 12}, {1, 17}, {1, 18}, {1, 19}, {2, 1},
                                   {2, 4},  {2, 7},  {2, 10}, {2, 13}, {2, 16}};
constexpr int kSeaMonsterDimY = 3;
constexpr int kSeaMonsterDimX = 20;

bool ContainsSeaMonsterAt(const BitSquare& image, int y, int x) {
  for (const Offsets offsets : kSeaMonster) {
    if (!image.Get(y + offsets.y, x + offsets.x)) return false;
  }
  return true;
}

bool ContainsSeaMonster(const BitSquare& image) {
  for (int y = 0; y < image.dim() - kSeaMonsterDimY; ++y) {
    for (int x = 0; x < image.dim() - kSeaMonsterDimX; ++x) {
      if (ContainsSeaMonsterAt(image, y, x)) return true;
    }
  }
  return false;
}

void RemoveSeaMonsterIfExistsAt(BitSquare* image, int y, int x) {
  if (!ContainsSeaMonsterAt(*image, y, x)) return;
  for (const Offsets offsets : kSeaMonster) {
    image->Set(y + offsets.y, x + offsets.x, false);
  }
}

void RemoveSeaMonsters(BitSquare* image) {
  for (int y = 0; y < image->dim() - kSeaMonsterDimY; ++y) {
    for (int x = 0; x < image->dim() - kSeaMonsterDimX; ++x) {
      RemoveSeaMonsterIfExistsAt(image, y, x);
    }
  }
}

}  // namespace

int main(int argc, char** argv) {
  CHECK(2 == argc);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);
  std::vector<std::vector<std::string>> line_groups =
      aoc2020::SplitByEmptyStrings(std::move(lines));

  std::vector<Tile> tiles;
  for (const auto& line_group : line_groups) {
    tiles.emplace_back(line_group);
  }

  std::vector<std::vector<BitSquare>> solution = SolvePuzzle(std::move(tiles));
  BitSquare assembled = Assemble(solution);
  for (BitSquare orientation : assembled.AllOrientations()) {
    if (ContainsSeaMonster(orientation)) {
      RemoveSeaMonsters(&orientation);
      std::cout << orientation.CountOnes() << "\n";
      return 0;
    }
  }

  std::cout << "Didn't find sea monster\n";
  return 1;
}
