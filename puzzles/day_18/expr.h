#ifndef PUZZLES_DAY_18_EXPR_H_
#define PUZZLES_DAY_18_EXPR_H_

#include <cstdint>
#include <memory>
#include <utility>

#include "absl/memory/memory.h"
#include "absl/strings/string_view.h"
#include "util/check.h"

namespace aoc2020::newmath {

class Expr {
 public:
  virtual ~Expr() = default;

  virtual std::int64_t Eval() const = 0;
};

class Literal final : public Expr {
 public:
  explicit Literal(std::int64_t value) : value_(value) {}

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

class Add final : public Expr {
 public:
  explicit Add(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
      : left_(std::move(left)), right_(std::move(right)) {}

  ~Add() override = default;

  std::int64_t Eval() const override { return left_->Eval() + right_->Eval(); }

 private:
  std::unique_ptr<Expr> left_, right_;
};

class Multiply final : public Expr {
 public:
  explicit Multiply(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
      : left_(std::move(left)), right_(std::move(right)) {}

  ~Multiply() override = default;

  std::int64_t Eval() const override { return left_->Eval() * right_->Eval(); }

 private:
  std::unique_ptr<Expr> left_, right_;
};

}  // namespace aoc2020::newmath

#endif  // PUZZLES_DAY_18_EXPR_H_
