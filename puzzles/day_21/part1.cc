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

  const absl::flat_hash_set<std::string>& possible_ingredients() const {
    return possible_ingredients_;
  }

 private:
  std::string name_;
  absl::flat_hash_set<std::string> possible_ingredients_;
};

int CountIngredientsInSet(absl::flat_hash_set<std::string>& ingredient_set,
                          const std::vector<ParsedFood>& foods) {
  int total = 0;
  for (const ParsedFood& food : foods) {
    for (const std::string& ingredient : food.ingredients) {
      total += ingredient_set.contains(ingredient);
    }
  }
  return total;
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

  absl::erase_if(ingredients, [&allergens](const std::string& ingredient) {
    for (const Allergen& allergen : allergens) {
      if (allergen.possible_ingredients().contains(ingredient)) {
        return true;
      }
    }
    return false;
  });

  std::cout << CountIngredientsInSet(ingredients, foods) << "\n";

  return 0;
}
