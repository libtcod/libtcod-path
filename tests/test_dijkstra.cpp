#define TCODPATH_ValueType int16_t
#define TCODPATH_IndexType int16_t

#include <libtcod-path/uniform_cost_search.h>

#include <array>
#include <catch2/catch_all.hpp>
#include <limits>
#include <stdexcept>

#include "common.h"

TEST_CASE("TCODPATH_dijkstra", "") {
  auto distance = Map2D({10, 10}, std::numeric_limits<Map2D<>::value_type>::max());
  distance[{0, 0}] = 0;
  auto map = Map2D({10, 10}, 1);
  map[{1, 1}] = 0;
  auto graph = TCODPATH_Graph{};
  graph.basic2d = TCODPATH_GraphBasic2D{
      TCODPATH_GRAPH_BASIC2D,
      map.c_data(),
      2,
      3,
  };
  TCODPATH_dijkstra(&graph, distance.c_data(), nullptr);
  REQUIRE(distance[{0, 0}] == 0);
  REQUIRE(distance[{1, 0}] == 2);
  REQUIRE(distance[{2, 0}] == 4);
  REQUIRE(distance[{1, 1}] == std::numeric_limits<Map2D<>::value_type>::max());
  REQUIRE(distance[{2, 1}] == 5);
}
