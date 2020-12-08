#include <cstdio>
#include <cstring>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/strings/string_view.h"
#include "absl/types/optional.h"
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

absl::optional<int> RunProgramUntilEnd(
    const std::vector<Instruction>& program) {
  int accumulator = 0;
  absl::flat_hash_set<std::size_t> visited_instructions;
  std::size_t current_instruction = 0;
  for (;;) {
    if (current_instruction == program.size()) return accumulator;
    if (!visited_instructions.insert(current_instruction).second) {
      return absl::nullopt;
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

int TryAllOpcodeFlips(std::vector<Instruction> program) {
  for (Instruction& instruction : program) {
    switch (instruction.opcode) {
      case Opcode::kAcc:
        continue;
      case Opcode::kJmp: {
        instruction.opcode = Opcode::kNop;
        absl::optional<int> maybe_result = RunProgramUntilEnd(program);
        if (maybe_result.has_value()) return *maybe_result;
        instruction.opcode = Opcode::kJmp;
        continue;
      }
      case Opcode::kNop: {
        instruction.opcode = Opcode::kJmp;
        absl::optional<int> maybe_result = RunProgramUntilEnd(program);
        if (maybe_result.has_value()) return *maybe_result;
        instruction.opcode = Opcode::kNop;
        continue;
      }
    }
  }
  CHECK_FAIL();
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

  std::cout << TryAllOpcodeFlips(program) << "\n";

  return 0;
}
