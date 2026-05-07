#define TCODPATH_ValueType int16_t
#define TCODPATH_VALUE_MAX INT16_MAX
#define TCODPATH_VALUE_MIN INT16_MIN
#define TCODPATH_IndexType int16_t

#include <libtcod-path/flow_tools.h>
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

  auto flow_data = std::vector<TCODPATH_IndexType>(distance.get_shape().at(0) * distance.get_shape().at(1) * 2);
  auto flow_shape = std::array<TCODPATH_IndexType, 3>{distance.get_shape().at(0), distance.get_shape().at(1), 2};
  auto flow_map = TCODPATH_Map{};
  TCODPATH_map_init_contigious_from(
      &flow_map,
      3,
      flow_shape.data(),
      static_cast<int>(sizeof(TCODPATH_IndexType)) * (std::is_signed_v<TCODPATH_IndexType> ? -1 : 1),
      (void*)flow_data.data());
  TCODPATH_flow_reset(&flow_map);

  TCODPATH_dijkstra(&graph, distance.c_data(), &flow_map);
  REQUIRE(distance[{0, 0}] == 0);
  REQUIRE(distance[{1, 0}] == 2);
  REQUIRE(distance[{2, 0}] == 4);
  REQUIRE(distance[{1, 1}] == std::numeric_limits<Map2D<>::value_type>::max());
  REQUIRE(distance[{2, 1}] == 5);

  static const auto EXPECTED_PATH = std::vector<std::array<TCODPATH_IndexType, 2>>{{0, 1}, {0, 0}};
  auto path = get_path(flow_map, {1, 2});
  REQUIRE(path == EXPECTED_PATH);
}
