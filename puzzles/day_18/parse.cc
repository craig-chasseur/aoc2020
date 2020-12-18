#include "puzzles/day_18/parse.h"

#include <memory>

#include "absl/memory/memory.h"
#include "absl/strings/ascii.h"
#include "absl/strings/string_view.h"
#include "puzzles/day_18/expr.h"
#include "util/check.h"

namespace aoc2020::newmath {
namespace {

std::unique_ptr<Expr> ParseUnary(absl::string_view* line,
                                 const bool add_has_precedence) {
  CHECK(!line->empty());
  std::unique_ptr<Expr> result;
  if (absl::ascii_isdigit(line->front())) {
    result = Literal::Parse(line);
  } else if (line->front() == '(') {
    absl::string_view sub_expression;
    int paren_count = 0;
    for (int pos = 0; pos < line->size(); ++pos) {
      switch ((*line)[pos]) {
        case '(':
          ++paren_count;
          break;
        case ')':
          --paren_count;
          break;
      }
      if (paren_count == 0) {
        sub_expression = line->substr(1, pos - 1);
        line->remove_prefix(pos + 1);
        break;
      }
    }
    CHECK(!sub_expression.empty());
    result = ParseFull(sub_expression, add_has_precedence);
  }

  if (line->empty()) return result;
  CHECK(line->front() == ' ');
  line->remove_prefix(1);
  return result;
}

std::unique_ptr<Expr> ParseUnaryOrAdd(absl::string_view* line,
                                      const bool add_has_precedence) {
  CHECK(!line->empty());
  std::unique_ptr<Expr> left;
  if (absl::ascii_isdigit(line->front()) || line->front() == '(') {
    left = ParseUnary(line, add_has_precedence);
  } else {
    CHECK_FAIL();
  }

  while (!line->empty()) {
    const char op = line->front();
    if (op == '*') return left;
    CHECK(op == '+');

    CHECK(line->size() > 1);
    CHECK((*line)[1] == ' ');
    line->remove_prefix(2);

    std::unique_ptr<Expr> right = ParseUnary(line, add_has_precedence);
    left = absl::make_unique<Add>(std::move(left), std::move(right));
  }
  return left;
}

}  // namespace

std::unique_ptr<Expr> ParseFull(absl::string_view line,
                                const bool add_has_precedence) {
  CHECK(!line.empty());
  std::unique_ptr<Expr> left = add_has_precedence
                                   ? ParseUnaryOrAdd(&line, true)
                                   : ParseUnary(&line, false);

  while (!line.empty()) {
    CHECK(line.size() > 1);
    const char op = line[0];
    CHECK(line[1] == ' ');
    line.remove_prefix(2);

    if (add_has_precedence) {
      CHECK(op == '*');
      std::unique_ptr<Expr> right = ParseUnaryOrAdd(&line, true);
      left = absl::make_unique<Multiply>(std::move(left), std::move(right));
    } else {
      std::unique_ptr<Expr> right = ParseUnary(&line, false);
      switch (op) {
        case '+':
          left = absl::make_unique<Add>(std::move(left), std::move(right));
          break;
        case '*':
          left = absl::make_unique<Multiply>(std::move(left), std::move(right));
          break;
        default:
          CHECK_FAIL();
      }
    }
  }
  return left;
}

}  // namespace aoc2020::newmath
