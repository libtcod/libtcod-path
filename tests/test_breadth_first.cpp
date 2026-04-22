
#include <libtcod-path/breadth_first_search.h>

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
  auto distance = Map2D(costs.get_shape(), std::numeric_limits<int>::max());
  distance[{0, 0}] = 0;
  auto graph = as_2d_graph(costs, 1, 1);
  TCODPATH_bfs(&graph, distance.c_data(), nullptr);
  REQUIRE(as_string(TEST_DATA) == as_string(distance));
}

TEST_CASE("TCODPATH_bfs large", "") {
  static constexpr auto SIZE = 2048;
  auto costs = Map2D{{SIZE, SIZE}, 1};
  auto distance = Map2D(costs.get_shape(), std::numeric_limits<int>::max());
  distance[{0, 0}] = 0;
  auto graph = as_2d_graph(costs, 1, 1);
  TCODPATH_bfs(&graph, distance.c_data(), nullptr);
  CHECK(distance[{0, 0}] == 0);
  CHECK(distance[{SIZE - 1, SIZE - 1}] == SIZE - 1);
}
