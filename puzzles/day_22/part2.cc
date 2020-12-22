#include <deque>
#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/string_view.h"
#include "util/check.h"
#include "util/io.h"

namespace {

enum class Player { kPlayer1, kPlayer2 };

struct Winner {
  Player player;
  std::deque<int> deck;
};

Winner RunGame(std::deque<int> player1_deck, std::deque<int> player2_deck) {
  absl::flat_hash_set<std::pair<std::deque<int>, std::deque<int>>>
      seen_configurations;
  while (!(player1_deck.empty() || player2_deck.empty())) {
    if (!seen_configurations.emplace(player1_deck, player2_deck).second) {
      return Winner{.player = Player::kPlayer1, .deck = player1_deck};
    }

    const int player1_card = player1_deck.front();
    player1_deck.pop_front();
    const int player2_card = player2_deck.front();
    player2_deck.pop_front();

    Player round_winner;
    if (player1_card <= player1_deck.size() &&
        player2_card <= player2_deck.size()) {
      std::deque<int> player1_subdeck(player1_deck.begin(),
                                      player1_deck.begin() + player1_card);
      std::deque<int> player2_subdeck(player2_deck.begin(),
                                      player2_deck.begin() + player2_card);
      round_winner =
          RunGame(std::move(player1_subdeck), std::move(player2_subdeck))
              .player;
    } else if (player1_card < player2_card) {
      round_winner = Player::kPlayer2;
    } else {
      round_winner = Player::kPlayer1;
    }

    if (round_winner == Player::kPlayer1) {
      player1_deck.push_back(player1_card);
      player1_deck.push_back(player2_card);
    } else {
      player2_deck.push_back(player2_card);
      player2_deck.push_back(player1_card);
    }
  }

  return player1_deck.empty()
             ? Winner{.player = Player::kPlayer2, .deck = player2_deck}
             : Winner{.player = Player::kPlayer1, .deck = player1_deck};
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

  const Winner winner =
      RunGame(std::move(player1_deck), std::move(player2_deck));

  std::cout << Score(winner.deck) << "\n";

  return 0;
}
