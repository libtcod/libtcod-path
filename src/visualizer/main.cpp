#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <libtcod-path/graph_tools.h>
#include <libtcod-path/heuristic_tools.h>
#include <libtcod-path/map_tools.h>
#include <libtcod.h>

#include <fstream>
#include <libtcod-path/map.hpp>
#include <string>

static SDL_Window* g_window{};  // Active window
static SDL_Renderer* g_renderer{};  // Active renderer

static std::optional<tcod::Console> g_console{};  // Map console
static SDL_Texture* g_texture{};  // Map texture

struct Deleter {
  void operator()(SDL_Texture* texture) { SDL_DestroyTexture(texture); }
  void operator()(TCODPATH_Map* map) { TCODPATH_map_delete(map); }
};

using MapPtr = std::unique_ptr<TCODPATH_Map, Deleter>;

struct MapData {
  tcod::path::Map2D<> costs{};
  TCODPATH_Graph graph;
  tcod::path::Map2D<> distance{};
  MapPtr flow{};
};

static MapData g_map_data{};

/// @brief Load a map from `path` and make it active.
/// @details See specification: https://movingai.com/benchmarks/formats.html
void load_map(const char* path) {
  auto file = std::ifstream{path, std::ios_base::in};
  std::string line{};
  std::getline(file, line);
  assert(line == std::string("type octile"));

  std::getline(file, line, ' ');
  assert(line == std::string("height"));
  int height{};
  file >> height;
  file.get();
  std::getline(file, line, ' ');
  assert(line == std::string("width"));
  int width{};
  file >> width;
  file.get();
  std::getline(file, line);
  assert(line == std::string("map"));

  // Unpack map data into console
  g_console = tcod::Console{width, height};
  auto& console = g_console.value();
  for (auto& it : console) it.ch = '@';
  for (int y = 0; y < height; ++y) {
    std::getline(file, line);
    for (int x = 0; x < width; ++x) console.at({x, y}).ch = line.at(x);
  }
  g_map_data.costs = tcod::path::Map2D{{height, width}};
  g_map_data.distance = tcod::path::Map2D{{height, width}};
  // Convert console into costs
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      switch (console.at({x, y}).ch) {
        case '.':  // passable terrain
        case 'G':  // passable terrain
          g_map_data.costs[{y, x}] = 1;
          break;
        case 'T':  // trees (impassable)
        case 'S':  // swamp (passable from regular terrain)
        case 'W':  // water (traversable, but not passable from terrain)
        case '@':  // out of bounds
        case 'O':  // out of bounds
        default:
          g_map_data.costs[{y, x}] = 0;
          break;
      }
    }
  }
  SDL_SetWindowSize(g_window, width, height);

  g_map_data.graph.basic2d = TCODPATH_GraphBasic2D{
      .type = TCODPATH_GRAPH_BASIC2D, .map = g_map_data.costs.c_data(), .cardinal = 2, .diagonal = 3};

  auto flow_shape = std::vector<int>{height, width, 2};
  g_map_data.flow = MapPtr{TCODPATH_map_new(3, flow_shape.data(), -2)};
}

SDL_AppResult SDL_AppInit(void**, [[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
  SDL_CreateWindowAndRenderer("Libtcod-path visualizer", 720, 480, SDL_WINDOW_RESIZABLE, &g_window, &g_renderer);
  return SDL_APP_CONTINUE;
}
SDL_AppResult SDL_AppIterate(void*) {
  SDL_SetRenderDrawColorFloat(g_renderer, 0, 0, 0, 1);
  SDL_RenderClear(g_renderer);
  if (!g_console.has_value()) {
    // render debug grid
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
  } else {
    const auto& console = g_console.value();
    if (g_texture) {
      float w, h = 0;
      SDL_GetTextureSize(g_texture, &w, &h);
      if (w != console.get_width() || h != console.get_height()) {
        SDL_DestroyTexture(g_texture);
        g_texture = nullptr;
      }
    }
    if (!g_texture) {
      g_texture = SDL_CreateTexture(
          g_renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, console.get_width(), console.get_height());
      assert(g_texture);
      SDL_SetTextureScaleMode(g_texture, SDL_SCALEMODE_NEAREST);
    }
    auto pixels_rgb = std::vector<TCOD_ColorRGB>{};
    pixels_rgb.reserve(console.get_width() * console.get_height());
    for (int y = 0; y < console.get_height(); ++y) {
      for (int x = 0; x < console.get_width(); ++x) {
        switch (console.at({x, y}).ch) {
          case '.':  // passable terrain
          case 'G':  // passable terrain
            pixels_rgb.push_back({255, 255, 255});
            break;
          case 'T':  // trees (impassable)
          case 'S':  // swamp (passable from regular terrain)
          case 'W':  // water (traversable, but not passable from terrain)
          case '@':  // out of bounds
          case 'O':  // out of bounds
          default:
            pixels_rgb.push_back({0, 0, 0});
            break;
        }
      }
    }
    SDL_UpdateTexture(g_texture, NULL, pixels_rgb.data(), console.get_width() * 3);

    SDL_RenderTexture(g_renderer, g_texture, NULL, NULL);
  }
  SDL_RenderPresent(g_renderer);
  return SDL_APP_CONTINUE;
}
SDL_AppResult SDL_AppEvent(void*, SDL_Event* event) {
  switch (event->type) {
    case SDL_EVENT_QUIT:
      return SDL_APP_SUCCESS;
    case SDL_EVENT_DROP_FILE:
      load_map(event->drop.data);
      break;
    default:
      break;
  }
  return SDL_APP_CONTINUE;
}
void SDL_AppQuit(void*, SDL_AppResult) {}
