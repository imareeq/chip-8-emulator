#include <SDL3/SDL.h>

#include <memory>
#include <span>
#include <string>

class platform {
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

  platform(std::string title, int window_height, int window_width, int texture_height, int texture_width);

  auto update(const std::span<uint32_t> buffer, int pitch) -> void;
  auto process_input(std::span<uint8_t> input_keys) const -> process_status;
};