#include <cstdio>
#include <cstring>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/strings/string_view.h"
#include "util/check.h"
#include "util/io.h"

namespace {

enum class Opcode {
  kAcc,
  kJmp,
  kNop,
};

struct Instruction {
  Opcode opcode = Opcode::kNop;
  int argument = 0;
};

Instruction ParseInstruction(absl::string_view instruction_str) {
  Instruction parsed;
  char opcode_str[4];
  CHECK(2 == std::sscanf(instruction_str.data(), "%3s %d", opcode_str,
                         &parsed.argument));
  if (std::strcmp(opcode_str, "acc") == 0) {
    parsed.opcode = Opcode::kAcc;
  } else if (std::strcmp(opcode_str, "jmp") == 0) {
    parsed.opcode = Opcode::kJmp;
  } else if (std::strcmp(opcode_str, "nop") == 0) {
    parsed.opcode = Opcode::kNop;
  } else {
    CHECK_FAIL();
  }
  return parsed;
}

int RunProgramUntilLoop(const std::vector<Instruction>& program) {
  int accumulator = 0;
  absl::flat_hash_set<std::size_t> visited_instructions;
  std::size_t current_instruction = 0;
  for (;;) {
    if (!visited_instructions.insert(current_instruction).second) {
      return accumulator;
    }

    const Instruction current = program[current_instruction];
    switch (current.opcode) {
      case Opcode::kAcc:
        accumulator += current.argument;
        ++current_instruction;
        break;
      case Opcode::kJmp:
        current_instruction += current.argument;
        break;
      case Opcode::kNop:
        ++current_instruction;
        break;
    }
  }
}

}  // namespace

int main(int argc, char** argv) {
  CHECK(argc == 2);
  std::vector<std::string> lines = aoc2020::ReadLinesFromFile(argv[1]);

  std::vector<Instruction> program;
  program.reserve(lines.size());
  for (const absl::string_view line : lines) {
    program.push_back(ParseInstruction(line));
  }

  std::cout << RunProgramUntilLoop(program) << "\n";

  return 0;
}
