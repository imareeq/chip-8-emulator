#include "chip_8.h"

#include <chrono>
#include <fstream>
#include <ranges>

namespace c8 {

chip_8::chip_8()
    : random_generator(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()))
    , random_byte(0u, 255u) {
  for (const auto& index : std::views::iota(0u, FONTSET_SIZE)) {
    memory[FONTSET_START_ADDRESS + index] = fontset[index];
  }

  opcode_table[0x0] = &chip_8::table_0;
  opcode_table[0x1] = &chip_8::op_1NNN;
  opcode_table[0x2] = &chip_8::op_2NNN;
  opcode_table[0x3] = &chip_8::op_3XNN;
  opcode_table[0x4] = &chip_8::op_4XNN;
  opcode_table[0x5] = &chip_8::op_5XY0;
  opcode_table[0x6] = &chip_8::op_6XNN;
  opcode_table[0x7] = &chip_8::op_7XNN;
  opcode_table[0x8] = &chip_8::table_8;
  opcode_table[0x9] = &chip_8::op_9XY0;
  opcode_table[0xA] = &chip_8::op_ANNN;
  opcode_table[0xB] = &chip_8::op_BNNN;
  opcode_table[0xC] = &chip_8::op_CXNN;
  opcode_table[0xD] = &chip_8::op_DXYN;
  opcode_table[0xE] = &chip_8::table_E;
  opcode_table[0xF] = &chip_8::table_F;

  for (const auto& index : std::views::iota(0u, 0xEu + 1u)) {
    opcode_table_0[index] = &chip_8::opcode_null;
    opcode_table_8[index] = &chip_8::opcode_null;
    opcode_table_E[index] = &chip_8::opcode_null;
  }

  opcode_table_0[0x0] = &chip_8::op_00E0;
  opcode_table_0[0xE] = &chip_8::op_00EE;

  opcode_table_8[0x0] = &chip_8::op_8XY0;
  opcode_table_8[0x1] = &chip_8::op_8XY1;
  opcode_table_8[0x2] = &chip_8::op_8XY2;
  opcode_table_8[0x3] = &chip_8::op_8XY3;
  opcode_table_8[0x4] = &chip_8::op_8XY4;
  opcode_table_8[0x5] = &chip_8::op_8XY5;
  opcode_table_8[0x6] = &chip_8::op_8XY6;
  opcode_table_8[0x7] = &chip_8::op_8XY7;
  opcode_table_8[0xE] = &chip_8::op_8XYE;

  opcode_table_E[0x1] = &chip_8::op_EXA1;
  opcode_table_E[0xE] = &chip_8::op_EX9E;

  for (const auto& index : std::views::iota(0u, 0x65u + 1u)) {
    opcode_table_F[index] = &chip_8::opcode_null;
  }

  opcode_table_F[0x07] = &chip_8::op_FX07;
  opcode_table_F[0x0A] = &chip_8::op_FX0A;
  opcode_table_F[0x15] = &chip_8::op_FX15;
  opcode_table_F[0x18] = &chip_8::op_FX18;
  opcode_table_F[0x1E] = &chip_8::op_FX1E;
  opcode_table_F[0x29] = &chip_8::op_FX29;
  opcode_table_F[0x33] = &chip_8::op_FX33;
  opcode_table_F[0x55] = &chip_8::op_FX55;
  opcode_table_F[0x65] = &chip_8::op_FX65;
}

auto chip_8::load_rom(const std::string file_path) -> void {
  std::ifstream file(file_path, std::ios::binary | std::ios::ate);

  if (file.is_open()) {
    auto size = file.tellg();
    auto buffer = std::string{};
    buffer.reserve(size);
    file.seekg(0);

    file.read(buffer.data(), size);
    file.close();

    for (const auto& i : std::views::iota(0, size)) {
      memory[ROM_START_ADDRESS + i] = buffer[i];
    }
  } else {
    memory[ROM_START_ADDRESS] = 0x00;
    memory[ROM_START_ADDRESS + 1] = 0xEE;
  }
}

auto chip_8::cycle() -> void {
  opcode = (memory[program_counter] << 8u) | memory[program_counter + 1];
  program_counter += 2;

  std::invoke(opcode_table[(opcode & OPCODE_MASK) >> OPCODE_SHIFT], this);

  if (delay_timer > 0) --delay_timer;
  if (sound_timer > 0) --sound_timer;
}

/* ========== OP CODES ========== */
auto chip_8::op_00E0() -> void { display_memory.fill(0); }

auto chip_8::op_00EE() -> void { program_counter = stack[--pointer]; }

auto chip_8::op_0NNN() -> void {
  // skipped according to:
  // https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#:~:text=0NNN%3A%20Execute%20machine%20language%20routinePermalink,of%20those%20computers%2C%20skip%20this%20one.
}

auto chip_8::op_1NNN() -> void { program_counter = opcode & NNN_MASK; }

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

  for (const auto& row : std::views::iota(0u, height)) {
    const auto sprite_byte = memory[index_register + row];
    for (const auto& col : std::views::iota(0u, SPRITE_WIDTH)) {
      const uint8_t sprite_pixel = sprite_byte & (MSB_MASK >> col);
      auto& screen_pixel = display_memory[(y_pos + row) * DISPLAY_WIDTH + (x_pos + col)];

      if (sprite_pixel) {
        if (screen_pixel == PIXEL_ON_VAL) {
          registers[VF_ADDRESS] = 1;
        }

        screen_pixel ^= PIXEL_ON_VAL;
      }
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

  for (const auto& index : std::views::iota(0u, 16u)) {
    if (input_keys[index]) {
      registers[VX] = static_cast<uint8_t>(index);
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

  for (const auto& i : std::views::iota(0u, VX + 1u)) {
    memory[index_register + i] = registers[i];
  }
}

auto chip_8::op_FX65() -> void {
  const uint8_t VX = (opcode & VX_MASK) >> VX_SHIFT;

  for (const auto& i : std::views::iota(0u, VX + 1u)) {
    registers[i] = memory[index_register + i];
  }
}

}  // namespace c8