#include <array>
#include <cstdint>
#include <random>
#include <span>
#include <string>
#include <vector>

namespace c8 {
class Chip_8 {
 private:
  static constexpr uint8_t SPRITE_WIDTH = 8;
  static constexpr uint8_t VF_ADDRESS = 0xF;
  static constexpr uint8_t NUM_REGISTERS = 16;
  static constexpr uint8_t STACK_SIZE = 16;
  static constexpr uint8_t NUM_INPUT_KEYS = 16;
  static constexpr uint8_t DISPLAY_HEIGHT = 32;
  static constexpr uint8_t DISPLAY_WIDTH = 64;
  static constexpr uint8_t FONTSET_SIZE = 80;
  static constexpr uint8_t FONTSET_START_ADDRESS = 0x50;
  static constexpr uint8_t REGISTER_MAX_VALUE = 255;
  static constexpr uint16_t ROM_START_ADDRESS = 0x200;
  static constexpr uint16_t MEMORY_BYTES = 4096;
  static constexpr uint32_t PIXEL_ON_VAL = 0xFFFFFFFF;

  static constexpr uint8_t LSB_MASK = 0x1;
  static constexpr uint8_t MSB_MASK = 0x80;
  static constexpr uint8_t VY_SHIFT = 4;
  static constexpr uint8_t VX_SHIFT = 8;
  static constexpr uint8_t OPCODE_SHIFT = 12;
  static constexpr uint16_t N_MASK = 0x000F;
  static constexpr uint16_t NN_MASK = 0x00FF;
  static constexpr uint16_t NNN_MASK = 0x0FFF;
  static constexpr uint16_t VX_MASK = 0x0F00;
  static constexpr uint16_t VY_MASK = 0x00F0;
  static constexpr uint16_t OPCODE_MASK = 0xF000;

  std::array<uint8_t, NUM_REGISTERS> registers{};
  std::array<uint8_t, MEMORY_BYTES> memory{};
  std::array<uint8_t, NUM_INPUT_KEYS> input_keys{};
  std::array<uint16_t, STACK_SIZE> stack{};
  std::array<uint32_t, DISPLAY_WIDTH * DISPLAY_HEIGHT> display_memory{};

  uint8_t pointer{};
  uint8_t delay_timer{};
  uint8_t sound_timer{};
  uint16_t index_register{};
  uint16_t program_counter{ROM_START_ADDRESS};
  uint16_t opcode{};

  std::default_random_engine random_generator;
  std::uniform_int_distribution<unsigned int> random_byte;

  std::array<uint8_t, FONTSET_SIZE> fontset = {
      0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
      0x20, 0x60, 0x20, 0x20, 0x70,  // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
      0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
      0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
      0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
      0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
      0xF0, 0x80, 0xF0, 0x80, 0x80   // F
  };

  using chip_8_func = void (Chip_8::*)();
  std::array<chip_8_func, 0xF + 1> opcode_table;
  std::array<chip_8_func, 0xE + 1> opcode_table_0;
  std::array<chip_8_func, 0xE + 1> opcode_table_8;
  std::array<chip_8_func, 0xE + 1> opcode_table_E;
  std::array<chip_8_func, 0x65 + 1> opcode_table_F;

  [[nodiscard]] auto inline get_vx() const -> uint8_t { return static_cast<uint8_t>((opcode & VX_MASK) >> VX_SHIFT); }
  [[nodiscard]] auto inline get_vy() const -> uint8_t { return static_cast<uint8_t>((opcode & VY_MASK) >> VY_SHIFT); }
  [[nodiscard]] auto inline get_n() const -> uint8_t { return static_cast<uint8_t>(opcode & N_MASK); }
  [[nodiscard]] auto inline get_nn() const -> uint8_t { return static_cast<uint8_t>(opcode & NN_MASK); }
  [[nodiscard]] auto inline get_nnn() const -> uint16_t { return static_cast<uint16_t>(opcode & NNN_MASK); }

 public:
  Chip_8();

  auto load_rom(const std::string file_path) -> void;
  auto cycle() -> void;

  [[nodiscard]] auto inline get_display_memory() const -> std::span<const uint32_t> { return display_memory; }
  auto inline enable_input_key(size_t index, bool is_enabled) -> void { input_keys[index] = is_enabled; }

  inline auto table_0() -> void { std::invoke(opcode_table_0[opcode & N_MASK], this); }
  inline auto table_8() -> void { std::invoke(opcode_table_8[opcode & N_MASK], this); }
  inline auto table_E() -> void { std::invoke(opcode_table_E[opcode & N_MASK], this); }
  inline auto table_F() -> void { std::invoke(opcode_table_F[opcode & NN_MASK], this); }
  inline auto opcode_null() -> void {}

  /*
    op codes docs at:
    https://github.com/trapexit/chip-8_documentation
  */

  auto op_00E0() -> void;  // CLS
  auto op_00EE() -> void;  // RET
  auto op_0NNN() -> void;  // CALL NNN
  auto op_1NNN() -> void;  // JMP NNN
  auto op_2NNN() -> void;  // CALL NNN
  auto op_3XNN() -> void;  // SE VX, NN
  auto op_4XNN() -> void;  // SNE VX, NN
  auto op_5XY0() -> void;  // SE VX, VY
  auto op_6XNN() -> void;  // LD VX, NN
  auto op_7XNN() -> void;  // ADD VX, NN
  auto op_8XY0() -> void;  // LD VX, VY
  auto op_8XY1() -> void;  // OR VX, VY
  auto op_8XY2() -> void;  // AND VX, VY
  auto op_8XY3() -> void;  // XOR VX, VY
  auto op_8XY4() -> void;  // ADD VX, VY
  auto op_8XY5() -> void;  // SUB VX, VY
  auto op_8XY6() -> void;  // SHR VX, VY
  auto op_8XY7() -> void;  // SUBN VX, VY
  auto op_8XYE() -> void;  // SHL VX, VY
  auto op_9XY0() -> void;  // SNE VX, VY
  auto op_ANNN() -> void;  // LD I, NNN
  auto op_BNNN() -> void;  // JMP V0, NNN
  auto op_CXNN() -> void;  // RND VX, NN
  auto op_DXYN() -> void;  // DRW VX, VY, N
  auto op_EX9E() -> void;  // SKP VX
  auto op_EXA1() -> void;  // SKNP VX
  auto op_FX07() -> void;  // LD VX, DT
  auto op_FX0A() -> void;  // LD VX, KEY
  auto op_FX15() -> void;  // LD DT, VX
  auto op_FX18() -> void;  // LD ST, VX
  auto op_FX1E() -> void;  // ADD I, VX
  auto op_FX29() -> void;  // LD I, FONT(VX)
  auto op_FX33() -> void;  // BCD VX
  auto op_FX55() -> void;  // LD [I], VX
  auto op_FX65() -> void;  // LD VX, [I]
};

// non member funcs
}  // namespace c8