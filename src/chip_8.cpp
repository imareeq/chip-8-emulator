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
  // TODO: implement when program loop is done
  return;
}

auto chip_8::op_00E0() -> void {
  display_memory.fill(0);
}

auto chip_8::op_00EE() -> void {
  program_counter = stack[--pointer];
}

auto chip_8::op_0NNN() -> void {
  // skipped according to:
  // https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#:~:text=0NNN%3A%20Execute%20machine%20language%20routinePermalink,of%20those%20computers%2C%20skip%20this%20one.
}

auto chip_8::op_1NNN() -> void {
  program_counter = opcode & NNN_MASK;
}

auto chip_8::op_2NNN() -> void {
  stack[pointer++] = program_counter;
  program_counter = opcode & NNN_MASK;
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

  if (registers[VX] == registers[VY]) {
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
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  const uint8_t VY = (opcode & VY_MASK) >> VY_SHIFT;

  const uint16_t sum = registers[VX] + registers[VY];

  registers[VF_ADDRESS] = sum > REGISTER_MAX_VALUE ? 1 : 0;
}

auto chip_8::op_8XY5() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  const uint8_t VY = (opcode & VY_MASK) >> VY_SHIFT;

  registers[VF_ADDRESS] = registers[VX] > registers[VY] ? 1 : 0;

  registers[VX] -= registers[VY];
}

auto chip_8::op_8XY6() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  registers[VF_ADDRESS] = (registers[VX] & LSB_MASK);

  registers[VX] >>= 1;
}

auto chip_8::op_8XY7() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  const uint8_t VY = (opcode & VY_MASK) >> VY_SHIFT;

  registers[VF_ADDRESS] = registers[VY] > registers[VX] ? 1 : 0;

  registers[VX] = registers[VY] = registers[VX];
}

auto chip_8::op_8XYE() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;

  registers[VF_ADDRESS] = (registers[VX] & 0x80u) >> 7u;

  registers[VX] <<= 1;
}

auto chip_8::op_9XY0() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  const uint8_t VY = (opcode & VY_MASK) >> VY_SHIFT;

  if (registers[VX] != registers[VY]) {
    program_counter += 2;
  }
}

auto chip_8::op_ANNN() -> void {
  const uint16_t address = (opcode & NNN_MASK);
  index_register = address;
}

auto chip_8::op_BNNN() -> void {
  const uint16_t address = (opcode & NNN_MASK);
  program_counter = registers[0] + address;
}

auto chip_8::op_CXNN() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  const uint8_t NN = opcode & NN_MASK;

  registers[VX] = random_byte(random_generator) & NN;
}

auto chip_8::op_DXYN() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  const uint8_t VY = (opcode & VY_MASK) >> VY_SHIFT;
  const uint8_t height = opcode & N_MASK;

  const uint8_t x_pos = registers[VX] % DISPLAY_WIDTH;
  const uint8_t y_pos = registers[VY] % DISPLAY_HEIGHT;

  registers[VF_ADDRESS] = 0;

  for (auto row : std::views::iota(0u, height)) {
    const auto sprite_byte = memory[index_register + row];
    for (auto col : std::views::iota(0u, SPRITE_WIDTH)) {
      const uint8_t sprite_pixel = sprite_byte & (MSB_MASK >> col);
      auto& screen_pixel = display_memory[(y_pos + row) * DISPLAY_WIDTH + (x_pos + col)];

      if (screen_pixel == PIXEL_ON_VAL) {
        registers[VF_ADDRESS] = 1;
      }

      screen_pixel ^= PIXEL_ON_VAL;
    }
  }
}

auto chip_8::op_EX9E() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  auto key = registers[VX];

  if (input_keys[key]) {
    program_counter += 2;
  }
}

auto chip_8::op_EXA1() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  auto key = registers[VX];

  if (!input_keys[key]) {
    program_counter += 2;
  }
}

auto chip_8::op_FX07() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  registers[VX] = delay_timer;
}

auto chip_8::op_FX0A() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;

  for (const auto index : std::views::iota(0u, 16u)) {
    if (input_keys[index]) {
      registers[VX] = index;
      return;
    }
  }

  program_counter -= 2;
}

auto chip_8::op_FX15() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  delay_timer = registers[VX];
}

auto chip_8::op_FX18() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  sound_timer = registers[VX];
}

auto chip_8::op_FX1E() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  index_register += registers[VX];
}

auto chip_8::op_FX29() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  const auto digit = registers[VX];

  index_register = FONTSET_START_ADDRESS + (5 * digit);
}

auto chip_8::op_FX33() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;
  auto value = registers[VX];

  memory[index_register + 2] = value % 10;
  value /= 10;

  memory[index_register + 1] = value % 10;
  value /= 10;

  memory[index_register] = value % 10;
}

auto chip_8::op_FX55() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;

  for (auto i : std::views::iota(0u, VX + 1u)) {
    memory[index_register + i] = registers[i];
  }
}

auto chip_8::op_FX65() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;

  for (auto i : std::views::iota(0u, VX + 1u)) {
    registers[i] = memory[index_register + i];
  }
}

}  // namespace c8