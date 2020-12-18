#ifndef PUZZLES_DAY_18_PARSE_H_
#define PUZZLES_DAY_18_PARSE_H_

#include <memory>

#include "absl/strings/string_view.h"
#include "puzzles/day_18/expr.h"

namespace aoc2020::newmath {

std::unique_ptr<Expr> ParseFull(absl::string_view line,
                                bool add_has_precedence);

}  // namespace aoc2020::newmath

#endif  // PUZZLES_DAY_18_PARSE_H_
