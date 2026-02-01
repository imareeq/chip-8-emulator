#include <chrono>
#include <iostream>

#include "platform.hpp"

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  auto video_scale = std::stoi(argv[1]);
  auto cycle_delay = std::stoi(argv[2]);
  auto file_name = std::string{argv[3]};

  auto platform = Platform(
      "Chip 8 Emulator", DISPLAY_HEIGHT * video_scale, DISPLAY_WIDTH * video_scale, DISPLAY_HEIGHT, DISPLAY_WIDTH);

  auto chip_8 = c8::Chip_8();
  chip_8.load_rom(file_name);

  auto video_pitch = static_cast<int>(sizeof(chip_8.get_display_memory()[0]) * DISPLAY_WIDTH);

  auto last_cycle_time = std::chrono::high_resolution_clock::now();

  while (platform.process_input(chip_8) != Platform::process_status::Quit) {
    const auto current_time = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(current_time - last_cycle_time).count();
    if (dt > cycle_delay) {
      last_cycle_time = current_time;

      chip_8.cycle();

      platform.update(chip_8.get_display_memory(), video_pitch);
    }
  }

  return 0;
}