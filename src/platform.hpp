#include <SDL3/SDL.h>

#include <memory>
#include <span>
#include <string>

#include "chip_8.h"

class Platform {
 private:
  struct sdl_initialiser {
    sdl_initialiser() { SDL_Init(SDL_INIT_VIDEO); }
    ~sdl_initialiser() { SDL_Quit(); }
  };

  struct sdl_destructor {
    auto operator()(SDL_Window* p) const -> void { SDL_DestroyWindow(p); }
    auto operator()(SDL_Renderer* p) const -> void { SDL_DestroyRenderer(p); }
    auto operator()(SDL_Texture* p) const -> void { SDL_DestroyTexture(p); }
  };

  sdl_initialiser sdl_init{};
  std::unique_ptr<SDL_Window, sdl_destructor> window;
  std::unique_ptr<SDL_Renderer, sdl_destructor> renderer;
  std::unique_ptr<SDL_Texture, sdl_destructor> texture;

 public:
  enum class process_status { Continue, Quit };

  Platform(std::string title, int window_height, int window_width, int texture_height, int texture_width);

  auto update(const std::span<const uint32_t> buffer, int pitch) -> void;
  auto process_input(c8::Chip_8& chip_8) const -> process_status;
};