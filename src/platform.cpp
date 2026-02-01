#include <platform.h>

namespace {
auto check_quit_event(SDL_Event& event) -> bool {
  return event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE);
}
}  // namespace

platform::platform(std::string title, int window_height, int window_width, int texture_height, int texture_width)
    : window{SDL_CreateWindow(title.data(), window_width, window_height, SDL_WINDOW_RESIZABLE)}
    , renderer{SDL_CreateRenderer(window.get(), nullptr)}
    , texture{SDL_CreateTexture(
          renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, texture_width, texture_height)} {}

auto platform::update(const std::span<uint32_t> buffer, int pitch) -> void {
  SDL_UpdateTexture(texture.get(), nullptr, buffer.data(), pitch);
  SDL_RenderClear(renderer.get());
  SDL_RenderTexture(renderer.get(), texture.get(), nullptr, nullptr);
  SDL_RenderPresent(renderer.get());
}

auto platform::process_input(std::span<uint8_t> input_keys) const -> platform::process_status {
  auto event = SDL_Event{};

  while (SDL_PollEvent(&event)) {
    if (check_quit_event(event)) return process_status::Quit;

    const auto key_value = event.type == SDL_EVENT_KEY_DOWN ? uint8_t{1} : uint8_t{0};

    switch (event.key.key) {
      case SDLK_X:
        input_keys[0] = key_value;
        break;
      case SDLK_1:
        input_keys[1] = key_value;
        break;
      case SDLK_2:
        input_keys[2] = key_value;
        break;
      case SDLK_3:
        input_keys[3] = key_value;
        break;
      case SDLK_Q:
        input_keys[4] = key_value;
        break;
      case SDLK_W:
        input_keys[5] = key_value;
        break;
      case SDLK_E:
        input_keys[6] = key_value;
        break;
      case SDLK_A:
        input_keys[7] = key_value;
        break;
      case SDLK_S:
        input_keys[8] = key_value;
        break;
      case SDLK_D:
        input_keys[9] = key_value;
        break;
      case SDLK_Z:
        input_keys[0xA] = key_value;
        break;
      case SDLK_C:
        input_keys[0xB] = key_value;
        break;
      case SDLK_4:
        input_keys[0xC] = key_value;
        break;
      case SDLK_R:
        input_keys[0xD] = key_value;
        break;
      case SDLK_F:
        input_keys[0xE] = key_value;
        break;
      case SDLK_V:
        input_keys[0xF] = key_value;
        break;
    }
  }

  return process_status::Continue;
};