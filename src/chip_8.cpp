#include "chip_8.h"
#include <fstream>
#include <ranges>

namespace c8
{
  auto chip_8::load_rom(const std::string file_path) -> void
  {
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);

    if (file.is_open())
    {
      auto size = file.tellg();
      auto buffer = std::string{};
      buffer.reserve(size);
      file.seekg(0);

      file.read(buffer.data(), size);
      file.close();

      for (auto i : std::views::iota(0, size))
      {
        this->memory[ROM_START_ADDRESS + i] = buffer[i];
      }
    }
  }
}