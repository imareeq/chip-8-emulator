#include <platform.hpp>

namespace {
auto check_quit_event(SDL_Event& event) -> bool {
  return event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE);
}
}  // namespace

Platform::Platform(std::string title, int window_height, int window_width, int texture_height, int texture_width)
    : window{SDL_CreateWindow(title.data(), window_width, window_height, SDL_WINDOW_RESIZABLE)}
    , renderer{SDL_CreateRenderer(window.get(), nullptr)}
    , texture{SDL_CreateTexture(
          renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, texture_width, texture_height)} {
  SDL_SetTextureScaleMode(texture.get(), SDL_SCALEMODE_NEAREST);
}

auto Platform::update(const std::span<const uint32_t> buffer, int pitch) -> void {
  SDL_UpdateTexture(texture.get(), nullptr, buffer.data(), pitch);
  SDL_RenderClear(renderer.get());
  SDL_RenderTexture(renderer.get(), texture.get(), nullptr, nullptr);
  SDL_RenderPresent(renderer.get());
}

auto Platform::process_input(c8::Chip_8& chip_8) const -> Platform::process_status {
  auto event = SDL_Event{};

  while (SDL_PollEvent(&event)) {
    if (check_quit_event(event)) return process_status::Quit;

    const auto is_key_enabled = event.type == SDL_EVENT_KEY_DOWN ? true : false;

    switch (event.key.key) {
      case SDLK_X:
        chip_8.enable_input_key(0, is_key_enabled);
        break;
      case SDLK_1:
        chip_8.enable_input_key(1, is_key_enabled);
        break;
      case SDLK_2:
        chip_8.enable_input_key(2, is_key_enabled);
        break;
      case SDLK_3:
        chip_8.enable_input_key(3, is_key_enabled);
        break;
      case SDLK_Q:
        chip_8.enable_input_key(4, is_key_enabled);
        break;
      case SDLK_W:
        chip_8.enable_input_key(5, is_key_enabled);
        break;
      case SDLK_E:
        chip_8.enable_input_key(6, is_key_enabled);
        break;
      case SDLK_A:
        chip_8.enable_input_key(7, is_key_enabled);
        break;
      case SDLK_S:
        chip_8.enable_input_key(8, is_key_enabled);
        break;
      case SDLK_D:
        chip_8.enable_input_key(9, is_key_enabled);
        break;
      case SDLK_Z:
        chip_8.enable_input_key(0xA, is_key_enabled);
        break;
      case SDLK_C:
        chip_8.enable_input_key(0xB, is_key_enabled);
        break;
      case SDLK_4:
        chip_8.enable_input_key(0xC, is_key_enabled);
        break;
      case SDLK_R:
        chip_8.enable_input_key(0xD, is_key_enabled);
        break;
      case SDLK_F:
        chip_8.enable_input_key(0xE, is_key_enabled);
        break;
      case SDLK_V:
        chip_8.enable_input_key(0xF, is_key_enabled);
        break;
    }
  }

  return process_status::Continue;
};