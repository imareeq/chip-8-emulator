#include "chip_8.h"

#include <fstream>
#include <ranges>

namespace c8 {
auto chip_8::load_rom(const std::string file_path) -> void {
  std::ifstream file(file_path, std::ios::binary | std::ios::ate);

  if (file.is_open()) {
    auto size = file.tellg();
    auto buffer = std::string{};
    buffer.reserve(size);
    file.seekg(0);

    file.read(buffer.data(), size);
    file.close();

    for (auto i : std::views::iota(0, size)) {
      memory[ROM_START_ADDRESS + i] = buffer[i];
    }
  } else {
    // TODO: maybe enumerate the opcodes or something.
    memory[ROM_START_ADDRESS] = 0x00;
    memory[ROM_START_ADDRESS + 1] = 0xEE;
  }
}

/* ========== OP CODES ========== */
auto chip_8::op_001N() -> void {
  // TODO: move to the end??
  return;
}

auto chip_8::op_00E0() -> void {
  display_memory.fill(0);
}

auto chip_8::op_00EE() -> void {
  program_counter = stack[--pointer];
}

auto chip_8::op_1NNN() -> void {
  program_counter = opcode & NNN_MASK;
}

auto chip_8::op_2NNN() -> void {
  stack[pointer++] = program_counter;
  program_counter = opcode && NNN_MASK;
}

auto chip_8::op_3XNN() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  const uint8_t NN = opcode & NN_MASK;

  if (registers[VX] == NN) {
    program_counter += 2;
  }
}

auto chip_8::op_4XNN() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  const uint8_t NN = opcode & NN_MASK;

  if (registers[VX] != NN) {
    program_counter += 2;
  }
}

auto chip_8::op_5XY0() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  const uint8_t VY = (opcode & VY_MASK) >> VY_SHIFT;

  if (registers[VX] = registers[VY]) {
    program_counter += 2;
  }
}

auto chip_8::op_6XNN() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  const uint8_t NN = opcode & NN_MASK;

  registers[VX] = NN;
}

auto chip_8::op_7XNN() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  const uint8_t NN = opcode & NN_MASK;

  registers[VX] += NN;
}

auto chip_8::op_8XY0() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  const uint8_t VY = (opcode & VY_MASK) >> VY_SHIFT;

  registers[VX] = registers[VY];
}

auto chip_8::op_8XY1() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  const uint8_t VY = (opcode & VY_MASK) >> VY_SHIFT;

  registers[VX] |= registers[VY];
}

auto chip_8::op_8XY2() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  const uint8_t VY = (opcode & VY_MASK) >> VY_SHIFT;

  registers[VX] &= registers[VY];
}

auto chip_8::op_8XY3() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  const uint8_t VY = (opcode & VY_MASK) >> VY_SHIFT;

  registers[VX] ^= registers[VY];
}

auto chip_8::op_8XY4() -> void {
  // TODO: add overflow
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  const uint8_t VY = (opcode & VY_MASK) >> VY_SHIFT;

  const uint16_t sum = registers[VX] + registers[VY];

  registers[VF_ADDRESS] = sum > REGISTER_MAX_VALUE ? 1 : 0;
}

}  // namespace c8