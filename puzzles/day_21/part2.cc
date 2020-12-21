#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "util/check.h"
#include "util/io.h"

namespace {

struct ParsedFood {
  explicit ParsedFood(absl::string_view line) {
    std::vector<absl::string_view> parts = absl::StrSplit(line, " (contains ");
    CHECK(parts.size() == 2);
    CHECK(parts.back().back() == ')');
    parts.back().remove_suffix(1);

    ingredients = absl::StrSplit(parts.front(), ' ');
    allergens = absl::StrSplit(parts.back(), ", ");
  }

  std::vector<std::string> ingredients;
  std::vector<std::string> allergens;
};

bool Contains(const std::vector<std::string>& vec, const std::string& element) {
  return std::find(vec.begin(), vec.end(), element) != vec.end();
}

class Allergen {
 public:
  explicit Allergen(std::string name, const std::vector<ParsedFood>& foods)
      : name_(std::move(name)) {
    bool set_initialized = false;
    for (const ParsedFood& food : foods) {
      if (Contains(food.allergens, name_)) {
        if (!set_initialized) {
          possible_ingredients_.insert(food.ingredients.begin(),
                                       food.ingredients.end());
          set_initialized = true;
        } else {
          absl::erase_if(possible_ingredients_,
                         [&food](const std::string& ingredient) {
                           return !Contains(food.ingredients, ingredient);
                         });
        }
      }
    }
  }

  const std::string& name() const { return name_; }

  const std::string& SinglePossibleIngredient() const {
    CHECK(possible_ingredients_.size() == 1);
    return *possible_ingredients_.begin();
  }

  const absl::flat_hash_set<std::string>& possible_ingredients() const {
    return possible_ingredients_;
  }

  void RemovePossibleIngredient(const std::string& ingredient) {
    possible_ingredients_.erase(ingredient);
  }

 private:
  std::string name_;
  absl::flat_hash_set<std::string> possible_ingredients_;
};

void SolveAllergens(std::vector<Allergen>& allergens) {
  std::vector<Allergen> solved;
  while (!allergens.empty()) {
    std::vector<Allergen> unsolved;
    bool any_new_solved = false;
    for (Allergen& allergen : allergens) {
      if (allergen.possible_ingredients().size() == 1) {
        solved.emplace_back(std::move(allergen));
        any_new_solved = true;
      } else {
        unsolved.emplace_back(std::move(allergen));
      }
    }
    CHECK(any_new_solved);
    for (Allergen& unsolved_allergen : unsolved) {
      for (const Allergen& solved_allergen : solved) {
        CHECK(solved_allergen.possible_ingredients().size() == 1);
        unsolved_allergen.RemovePossibleIngredient(
            solved_allergen.SinglePossibleIngredient());
      }
    }
    allergens = std::move(unsolved);
  }
  allergens = std::move(solved);
}

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);

  std::vector<ParsedFood> foods;
  for (absl::string_view line : lines) {
    foods.emplace_back(line);
  }

  absl::flat_hash_set<std::string> ingredients;
  absl::flat_hash_set<std::string> allergen_names;
  for (const ParsedFood& food : foods) {
    ingredients.insert(food.ingredients.begin(), food.ingredients.end());
    allergen_names.insert(food.allergens.begin(), food.allergens.end());
  }

  std::vector<Allergen> allergens;
  for (const std::string& allergen_name : allergen_names) {
    allergens.emplace_back(allergen_name, foods);
  }

  SolveAllergens(allergens);
  std::sort(
      allergens.begin(), allergens.end(),
      [](const Allergen& a, const Allergen& b) { return a.name() < b.name(); });

  std::cout << allergens.front().SinglePossibleIngredient();
  for (auto iter = allergens.begin() + 1; iter != allergens.end(); ++iter) {
    std::cout << ',' << iter->SinglePossibleIngredient();
  }
  std::cout << "\n";
 
  return 0;
}
