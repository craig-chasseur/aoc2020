#include <immintrin.h>

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/match.h"
#include "absl/strings/string_view.h"
#include "util/check.h"
#include "util/io.h"

namespace {

struct Mask {
  std::uint64_t floating_bits = 0;
  std::uint64_t set_bits = 0;
};

Mask ParseMask(absl::string_view mask_str) {
  CHECK(mask_str.size() == 36);
  Mask parsed;
  for (char c : mask_str) {
    parsed.floating_bits <<= 1;
    parsed.set_bits <<= 1;
    switch (c) {
      case 'X':
        parsed.floating_bits |= 1;
        break;
      case '0':
        parsed.set_bits |= 0;
        break;
      case '1':
        parsed.set_bits |= 1;
        break;
      default:
        CHECK_FAIL();
    }
  }
  return parsed;
}

struct StoreInst {
  std::uint64_t address = 0;
  std::uint64_t value = 0;
};

StoreInst ParseStore(absl::string_view store_line) {
  StoreInst parsed;
  CHECK(2 == std::sscanf(store_line.data(), "mem[%lu] = %lu", &parsed.address,
                         &parsed.value));
  return parsed;
}

class MemoryMachine {
 public:
  MemoryMachine() = default;

  void SetMask(const Mask& mask) { current_mask_ = mask; }

  void ExecStore(const StoreInst& store) {
    std::uint64_t address_base =
        (store.address | current_mask_.set_bits) & ~current_mask_.floating_bits;

    int mask_bits = __builtin_popcountll(current_mask_.floating_bits);
    for (std::uint64_t floating_bits_contiguous = 0;
         floating_bits_contiguous < (1 << mask_bits);
         ++floating_bits_contiguous) {
      const std::uint64_t address =
          _pdep_u64(floating_bits_contiguous, current_mask_.floating_bits) |
          address_base;
      sparse_memory_[address] = store.value;
    }
  }

  std::uint64_t SumMemory() const {
    std::uint64_t total = 0;
    for (const auto& addr_value : sparse_memory_) {
      total += addr_value.second;
    }
    return total;
  }

 private:
  Mask current_mask_;
  absl::flat_hash_map<std::uint64_t, std::uint64_t> sparse_memory_;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK(2 == argc);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);

  MemoryMachine machine;
  for (const absl::string_view line : lines) {
    if (absl::StartsWith(line, "mask = ")) {
      absl::string_view mask_str = line.substr(7);
      machine.SetMask(ParseMask(mask_str));
    } else if (absl::StartsWith(line, "mem[")) {
      machine.ExecStore(ParseStore(line));
    } else {
      CHECK_FAIL();
    }
  }

  std::cout << machine.SumMemory() << "\n";

  return 0;
}
