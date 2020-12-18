#include <cstdint>
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
  std::unique_ptr<Expr> left;
  if (absl::ascii_isdigit(line.front()) || line.front() == '(') {
    left = ParseUnary(&line);
  } else {
    CHECK_FAIL();
  }

  while (!line.empty()) {
    const char op = line.front();
    CHECK(line.size() > 1);
    CHECK(line[1] == ' ');
    line.remove_prefix(2);

    std::unique_ptr<Expr> right = ParseUnary(&line);
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

  return left;
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
