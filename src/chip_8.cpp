#include "chip_8.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <numeric>
namespace c8 {

Chip_8::Chip_8()
    : random_generator(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()))
    , random_byte(0u, 255u) {
  for (auto index = 0u; index < FONTSET_SIZE; ++index) {
    memory[FONTSET_START_ADDRESS + index] = fontset[index];
  }

  opcode_table[0x0] = &Chip_8::table_0;
  opcode_table[0x1] = &Chip_8::op_1NNN;
  opcode_table[0x2] = &Chip_8::op_2NNN;
  opcode_table[0x3] = &Chip_8::op_3XNN;
  opcode_table[0x4] = &Chip_8::op_4XNN;
  opcode_table[0x5] = &Chip_8::op_5XY0;
  opcode_table[0x6] = &Chip_8::op_6XNN;
  opcode_table[0x7] = &Chip_8::op_7XNN;
  opcode_table[0x8] = &Chip_8::table_8;
  opcode_table[0x9] = &Chip_8::op_9XY0;
  opcode_table[0xA] = &Chip_8::op_ANNN;
  opcode_table[0xB] = &Chip_8::op_BNNN;
  opcode_table[0xC] = &Chip_8::op_CXNN;
  opcode_table[0xD] = &Chip_8::op_DXYN;
  opcode_table[0xE] = &Chip_8::table_E;
  opcode_table[0xF] = &Chip_8::table_F;

  for (auto index = 0u; index <= 0xEu; ++index) {
    opcode_table_0[index] = &Chip_8::opcode_null;
    opcode_table_8[index] = &Chip_8::opcode_null;
    opcode_table_E[index] = &Chip_8::opcode_null;
  }

  opcode_table_0[0x0] = &Chip_8::op_00E0;
  opcode_table_0[0xE] = &Chip_8::op_00EE;

  opcode_table_8[0x0] = &Chip_8::op_8XY0;
  opcode_table_8[0x1] = &Chip_8::op_8XY1;
  opcode_table_8[0x2] = &Chip_8::op_8XY2;
  opcode_table_8[0x3] = &Chip_8::op_8XY3;
  opcode_table_8[0x4] = &Chip_8::op_8XY4;
  opcode_table_8[0x5] = &Chip_8::op_8XY5;
  opcode_table_8[0x6] = &Chip_8::op_8XY6;
  opcode_table_8[0x7] = &Chip_8::op_8XY7;
  opcode_table_8[0xE] = &Chip_8::op_8XYE;

  opcode_table_E[0x1] = &Chip_8::op_EXA1;
  opcode_table_E[0xE] = &Chip_8::op_EX9E;

  for (auto index = 0u; index <= 0x65u; ++index) {
    opcode_table_F[index] = &Chip_8::opcode_null;
  }

  opcode_table_F[0x07] = &Chip_8::op_FX07;
  opcode_table_F[0x0A] = &Chip_8::op_FX0A;
  opcode_table_F[0x15] = &Chip_8::op_FX15;
  opcode_table_F[0x18] = &Chip_8::op_FX18;
  opcode_table_F[0x1E] = &Chip_8::op_FX1E;
  opcode_table_F[0x29] = &Chip_8::op_FX29;
  opcode_table_F[0x33] = &Chip_8::op_FX33;
  opcode_table_F[0x55] = &Chip_8::op_FX55;
  opcode_table_F[0x65] = &Chip_8::op_FX65;
}

auto Chip_8::load_rom(const std::string file_path) -> void {
  std::ifstream file(file_path, std::ios::binary | std::ios::ate);

  if (file.is_open()) {
    auto size = file.tellg();
    auto buffer = std::string{};
    buffer.resize(static_cast<size_t>(size));
    file.seekg(0);

    file.read(buffer.data(), size);
    file.close();

    for (auto index = 0u; index < size; ++index) {
      memory[ROM_START_ADDRESS + index] = static_cast<uint8_t>(buffer[index]);
    }
  } else {
    std::cerr << "Invalid file path" << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

auto Chip_8::cycle() -> void {
  opcode = static_cast<uint16_t>((memory[program_counter] << 8u) | memory[program_counter + 1]);
  program_counter += 2;

  std::invoke(opcode_table[(opcode & OPCODE_MASK) >> OPCODE_SHIFT], this);

  if (delay_timer > 0) --delay_timer;
  if (sound_timer > 0) --sound_timer;
}

/* ========== OP CODES ========== */
auto Chip_8::op_00E0() -> void { display_memory.fill(0); }

auto Chip_8::op_00EE() -> void { program_counter = stack[--pointer]; }

auto Chip_8::op_0NNN() -> void {
  // skipped according to:
  // https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#:~:text=0NNN%3A%20Execute%20machine%20language%20routinePermalink,of%20those%20computers%2C%20skip%20this%20one.
}

auto Chip_8::op_1NNN() -> void { program_counter = opcode & NNN_MASK; }

auto Chip_8::op_2NNN() -> void {
  stack[pointer++] = program_counter;
  program_counter = opcode & NNN_MASK;
}

auto Chip_8::op_3XNN() -> void {
  const auto VX = get_vx();
  const auto NN = get_nn();

  if (registers[VX] == NN) {
    program_counter += 2;
  }
}

auto Chip_8::op_4XNN() -> void {
  const auto VX = get_vx();
  const auto NN = get_nn();

  if (registers[VX] != NN) {
    program_counter += 2;
  }
}

auto Chip_8::op_5XY0() -> void {
  const auto VX = get_vx();
  const auto VY = get_vy();

  if (registers[VX] == registers[VY]) {
    program_counter += 2;
  }
}

auto Chip_8::op_6XNN() -> void {
  const auto VX = get_vx();
  const auto NN = get_nn();

  registers[VX] = NN;
}

auto Chip_8::op_7XNN() -> void {
  const auto VX = get_vx();
  const auto NN = get_nn();

  registers[VX] += NN;
}

auto Chip_8::op_8XY0() -> void {
  const auto VX = get_vx();
  const auto VY = get_vy();

  registers[VX] = registers[VY];
}

auto Chip_8::op_8XY1() -> void {
  const auto VX = get_vx();
  const auto VY = get_vy();

  registers[VX] |= registers[VY];
}

auto Chip_8::op_8XY2() -> void {
  const auto VX = get_vx();
  const auto VY = get_vy();

  registers[VX] &= registers[VY];
}

auto Chip_8::op_8XY3() -> void {
  const auto VX = get_vx();
  const auto VY = get_vy();

  registers[VX] ^= registers[VY];
}

auto Chip_8::op_8XY4() -> void {
  const auto VX = get_vx();
  const auto VY = get_vy();

  const uint16_t sum = registers[VX] + registers[VY];

  registers[VF_ADDRESS] = sum > REGISTER_MAX_VALUE ? 1 : 0;
  registers[VX] = sum & 0xFFu;
}

auto Chip_8::op_8XY5() -> void {
  const auto VX = get_vx();
  const auto VY = get_vy();

  registers[VF_ADDRESS] = registers[VX] > registers[VY] ? 1 : 0;

  registers[VX] -= registers[VY];
}

auto Chip_8::op_8XY6() -> void {
  const auto VX = get_vx();
  registers[VF_ADDRESS] = (registers[VX] & LSB_MASK);

  registers[VX] >>= 1;
}

auto Chip_8::op_8XY7() -> void {
  const auto VX = get_vx();
  const auto VY = get_vy();

  registers[VF_ADDRESS] = registers[VY] > registers[VX] ? 1 : 0;

  registers[VX] = registers[VY] = registers[VX];
}

auto Chip_8::op_8XYE() -> void {
  const auto VX = get_vx();

  registers[VF_ADDRESS] = (registers[VX] & 0x80u) >> 7u;

  registers[VX] <<= 1;
}

auto Chip_8::op_9XY0() -> void {
  const auto VX = get_vx();
  const auto VY = get_vy();

  if (registers[VX] != registers[VY]) {
    program_counter += 2;
  }
}

auto Chip_8::op_ANNN() -> void {
  const auto address = get_nnn();
  index_register = address;
}

auto Chip_8::op_BNNN() -> void {
  const auto address = get_nnn();
  program_counter = registers[0] + address;
}

auto Chip_8::op_CXNN() -> void {
  const auto VX = get_vx();
  const auto NN = get_nn();

  registers[VX] = random_byte(random_generator) & NN;
}

auto Chip_8::op_DXYN() -> void {
  const auto VX = get_vx();
  const auto VY = get_vy();
  const auto height = get_n();

  const auto x_pos = static_cast<uint8_t>(registers[VX] % DISPLAY_WIDTH);
  const auto y_pos = static_cast<uint8_t>(registers[VY] % DISPLAY_HEIGHT);

  registers[VF_ADDRESS] = 0;

  for (auto row = 0u; row < height; ++row) {
    const auto sprite_byte = memory[index_register + row];
    for (auto col = 0u; col < SPRITE_WIDTH; ++col) {
      const auto sprite_pixel = static_cast<uint8_t>(sprite_byte & (MSB_MASK >> col));
      auto& screen_pixel =
          display_memory[((y_pos + row) % DISPLAY_HEIGHT) * DISPLAY_WIDTH + ((x_pos + col) % DISPLAY_WIDTH)];

      if (sprite_pixel) {
        if (screen_pixel == PIXEL_ON_VAL) {
          registers[VF_ADDRESS] = 1;
        }

        screen_pixel ^= PIXEL_ON_VAL;
      }
    }
  }
}

auto Chip_8::op_EX9E() -> void {
  const auto VX = get_vx();
  const auto key = registers[VX];

  if (input_keys[key]) {
    program_counter += 2;
  }
}

auto Chip_8::op_EXA1() -> void {
  const auto VX = get_vx();
  const auto key = registers[VX];

  if (!input_keys[key]) {
    program_counter += 2;
  }
}

auto Chip_8::op_FX07() -> void {
  const auto VX = get_vx();
  registers[VX] = delay_timer;
}

auto Chip_8::op_FX0A() -> void {
  const auto VX = get_vx();

  for (auto index = 0u; index < 16u; ++index) {
    if (input_keys[index]) {
      registers[VX] = static_cast<uint8_t>(index);
      return;
    }
  }

  program_counter -= 2;
}

auto Chip_8::op_FX15() -> void {
  const auto VX = get_vx();
  delay_timer = registers[VX];
}

auto Chip_8::op_FX18() -> void {
  const auto VX = get_vx();
  sound_timer = registers[VX];
}

auto Chip_8::op_FX1E() -> void {
  const auto VX = get_vx();
  index_register += registers[VX];
}

auto Chip_8::op_FX29() -> void {
  const auto VX = get_vx();
  const auto digit = registers[VX];

  index_register = FONTSET_START_ADDRESS + (5 * digit);
}

auto Chip_8::op_FX33() -> void {
  const auto VX = get_vx();
  auto value = registers[VX];

  memory[index_register + 2] = value % 10;
  value /= 10;

  memory[index_register + 1] = value % 10;
  value /= 10;

  memory[index_register] = value % 10;
}

auto Chip_8::op_FX55() -> void {
  const auto VX = get_vx();

  for (auto index = 0u; index <= VX; ++index) {
    memory[index_register + index] = registers[index];
  }
}

auto Chip_8::op_FX65() -> void {
  const auto VX = get_vx();

  for (auto index = 0u; index <= VX; ++index) {
    registers[index] = memory[index_register + index];
  }
}

}  // namespace c8