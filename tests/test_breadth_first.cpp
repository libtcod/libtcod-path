#define TCODPATH_ValueType int16_t
#define TCODPATH_VALUE_MAX INT16_MAX
#define TCODPATH_VALUE_MIN INT16_MIN
#define TCODPATH_IndexType int16_t

#include <libtcod-path/breadth_first_search.h>
#include <libtcod-path/flow_tools.h>

#include <array>
#include <catch2/catch_all.hpp>
#include <limits>
#include <stdexcept>

#include "common.h"

TEST_CASE("TCODPATH_bfs", "") {
  static const auto TEST_DATA = std::vector<std::string>{
      "0#44567",
      "1#34##7",
      "2234567",
  };
  auto costs = wall_costs_from_test_data(TEST_DATA);
  auto distance = Map2D(costs.get_shape(), std::numeric_limits<Map2D<>::value_type>::max());
  distance[{0, 0}] = 0;
  auto graph = as_2d_graph(costs, 1, 1);

  auto flow_data = std::vector<TCODPATH_IndexType>(distance.get_shape().at(0) * distance.get_shape().at(1) * 2);
  auto flow_shape = std::array<TCODPATH_IndexType, 3>{distance.get_shape().at(0), distance.get_shape().at(1), 2};
  auto flow_map = TCODPATH_Map{};
  TCODPATH_map_init_contigious_from(
      &flow_map,
      3,
      flow_shape.data(),
      static_cast<int>(sizeof(TCODPATH_IndexType)) * (std::is_signed_v<TCODPATH_IndexType> ? -1 : 1),
      static_cast<void*>(flow_data.data()));
  TCODPATH_flow_reset(&flow_map);

  TCODPATH_bfs(&graph, distance.c_data(), &flow_map);
  REQUIRE(as_string(TEST_DATA) == as_string(distance));

  static const auto EXPECTED_PATH = std::vector<std::array<TCODPATH_IndexType, 2>>{{1, 2}, {2, 1}, {1, 0}, {0, 0}};
  auto path = get_path(flow_map, {0, 2});
  REQUIRE(path == EXPECTED_PATH);
}

TEST_CASE("TCODPATH_bfs large", "[.slow]") {
  static constexpr auto SIZE = 2048;
  auto costs = Map2D{{SIZE, SIZE}, 1};
  auto distance = Map2D(costs.get_shape(), std::numeric_limits<Map2D<>::value_type>::max());
  distance[{0, 0}] = 0;
  auto graph = as_2d_graph(costs, 1, 1);
  TCODPATH_bfs(&graph, distance.c_data(), nullptr);
  CHECK(distance[{0, 0}] == 0);
  CHECK(distance[{SIZE - 1, SIZE - 1}] == SIZE - 1);
}
