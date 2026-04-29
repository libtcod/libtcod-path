#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <libtcod.h>

static SDL_Window* g_window{};
static SDL_Renderer* g_renderer{};

SDL_AppResult SDL_AppInit(void**, [[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  SDL_CreateWindowAndRenderer("Libtcod-path visualizer", 720, 480, SDL_WINDOW_RESIZABLE, &g_window, &g_renderer);
  return SDL_APP_CONTINUE;
}
SDL_AppResult SDL_AppIterate(void*) {
  // render debug grid
  SDL_SetRenderDrawColorFloat(g_renderer, 0, 0, 0, 1);
  SDL_RenderClear(g_renderer);
  int w, h;
  SDL_GetRenderOutputSize(g_renderer, &w, &h);
  SDL_SetRenderDrawColorFloat(g_renderer, 0.2f, 0.2f, 0.2f, 1);
  constexpr int TILE_SIZE = 4;
  for (int y = 0; y < h; y += TILE_SIZE) {
    for (int x = 0; x < w; x += TILE_SIZE) {
      if (((y / TILE_SIZE) ^ (x / TILE_SIZE)) & 1) continue;
      auto rect = SDL_FRect{(float)x, (float)y, TILE_SIZE, TILE_SIZE};
      SDL_RenderFillRect(g_renderer, &rect);
    }
  }
  SDL_RenderPresent(g_renderer);
  return SDL_APP_CONTINUE;
}
SDL_AppResult SDL_AppEvent(void*, SDL_Event* event) {
  switch (event->type) {
    case SDL_EVENT_QUIT:
      return SDL_APP_SUCCESS;

    default:
      break;
  }
  return SDL_APP_CONTINUE;
}
void SDL_AppQuit(void*, SDL_AppResult) {}
