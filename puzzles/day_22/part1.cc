#include <deque>
#include <string>
#include <vector>

#include "absl/strings/numbers.h"
#include "absl/strings/string_view.h"
#include "util/check.h"
#include "util/io.h"

namespace {

void RunGame(std::deque<int>* player1_deck, std::deque<int>* player2_deck) {
  while (!(player1_deck->empty() || player2_deck->empty())) {
    if (player1_deck->front() < player2_deck->front()) {
      player2_deck->push_back(player2_deck->front());
      player2_deck->push_back(player1_deck->front());
    } else {
      player1_deck->push_back(player1_deck->front());
      player1_deck->push_back(player2_deck->front());
    }
    player1_deck->pop_front();
    player2_deck->pop_front();
  }
}

std::deque<int> ParseDeck(const std::vector<std::string>& strs) {
  std::deque<int> deck;
  for (auto iter = strs.begin() + 1; iter != strs.end(); ++iter) {
    int card = 0;
    CHECK(absl::SimpleAtoi(*iter, &card));
    deck.push_back(card);
  }
  return deck;
}

int Score(const std::deque<int>& deck) {
  int mul = 1;
  int sum = 0;
  for (auto iter = deck.rbegin(); iter != deck.rend(); ++iter) {
    sum += *iter * (mul++);
  }
  return sum;
}

}  // namespace

int main(int argc, char** argv) {
  CHECK(2 == argc);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);
  std::vector<std::vector<std::string>> decks =
      aoc2020::SplitByEmptyStrings(std::move(lines));
  CHECK(2 == decks.size());

  CHECK(decks.front().front() == "Player 1:");
  std::deque<int> player1_deck = ParseDeck(decks.front());

  CHECK(decks.back().front() == "Player 2:");
  std::deque<int> player2_deck = ParseDeck(decks.back());

  RunGame(&player1_deck, &player2_deck);

  const int score =
      player1_deck.empty() ? Score(player2_deck) : Score(player1_deck);
  std::cout << score << "\n";

  return 0;
}
