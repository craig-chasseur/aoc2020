#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "absl/memory/memory.h"
#include "absl/strings/ascii.h"
#include "absl/strings/string_view.h"
#include "util/check.h"
#include "util/io.h"

namespace {

class Expr {
 public:
  virtual ~Expr() = default;

  virtual std::int64_t Eval() const = 0;
};

class Literal : public Expr {
 public:
  explicit Literal(int value) : value_(value) {}

  ~Literal() override = default;

  std::int64_t Eval() const override { return value_; }

  static std::unique_ptr<Literal> Parse(absl::string_view* line) {
    CHECK(!line->empty());
    std::unique_ptr<Literal> parsed =
        absl::make_unique<Literal>(line->front() - '0');
    line->remove_prefix(1);
    return parsed;
  }

 private:
  std::int64_t value_;
};

class Add : public Expr {
 public:
  explicit Add(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
      : left_(std::move(left)), right_(std::move(right)) {}

  ~Add() override = default;

  std::int64_t Eval() const override { return left_->Eval() + right_->Eval(); }

 private:
  std::unique_ptr<Expr> left_, right_;
};

class Multiply : public Expr {
 public:
  explicit Multiply(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
      : left_(std::move(left)), right_(std::move(right)) {}

  ~Multiply() override = default;

  std::int64_t Eval() const override { return left_->Eval() * right_->Eval(); }

 private:
  std::unique_ptr<Expr> left_, right_;
};

std::unique_ptr<Expr> ParseFull(absl::string_view line);

std::unique_ptr<Expr> ParseUnary(absl::string_view* line) {
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
    result = ParseFull(sub_expression);
  }

  if (line->empty()) return result;
  CHECK(line->front() == ' ');
  line->remove_prefix(1);
  return result;
}

std::unique_ptr<Expr> ParseFull(absl::string_view line) {
  CHECK(!line.empty());
  std::vector<std::unique_ptr<Expr>> operands;
  if (absl::ascii_isdigit(line.front()) || line.front() == '(') {
    operands.emplace_back(ParseUnary(&line));
  } else {
    CHECK_FAIL();
  }

  std::vector<char> ops;
  while (!line.empty()) {
    ops.push_back(line.front());
    CHECK(line.size() > 1);
    CHECK(line[1] == ' ');
    line.remove_prefix(2);
    operands.emplace_back(ParseUnary(&line));
  }
  CHECK(ops.size() + 1 == operands.size());

  for (auto op_iter = std::find(ops.begin(), ops.end(), '+');
       op_iter != ops.end(); op_iter = std::find(op_iter, ops.end(), '+')) {
    const int pos = op_iter - ops.begin();
    operands[pos] = absl::make_unique<Add>(std::move(operands[pos]),
                                           std::move(operands[pos + 1]));
    op_iter = ops.erase(op_iter);
    operands.erase(operands.begin() + pos + 1);
  }

  std::unique_ptr<Expr> result = std::move(operands.front());
  for (auto operand_it = operands.begin() + 1; operand_it != operands.end();
       ++operand_it) {
    result =
        absl::make_unique<Multiply>(std::move(result), std::move(*operand_it));
  }
  return result;
}

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);

  std::int64_t sum = 0;
  for (absl::string_view line : lines) {
    sum += ParseFull(line)->Eval();
  }
  std::cout << sum << "\n";

  return 0;
}
