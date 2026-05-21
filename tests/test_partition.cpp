#define TCODPATH_ValueType int16_t
#define TCODPATH_VALUE_MAX INT16_MAX
#define TCODPATH_VALUE_MIN INT16_MIN
#define TCODPATH_IndexType int16_t

#include <libtcod-path/graph_types.h>
#include <libtcod-path/partition.h>

#include <array>
#include <catch2/catch_all.hpp>
#include <limits>
#include <stdexcept>

#include "common.h"

TEST_CASE("TCODPATH_partition", "") {
  static const auto TEST_DATA = std::vector<std::string>{
      "1110220",
      "1110203",
      "1110033",
  };
  static const auto shape = std::array{
      static_cast<TCODPATH_IndexType>(TEST_DATA.size()), static_cast<TCODPATH_IndexType>(TEST_DATA.at(0).size())};
  auto costs = Map2D(shape, -1);
  for (TCODPATH_IndexType y = 0; y < TEST_DATA.size(); ++y) {
    for (TCODPATH_IndexType x = 0; x < TEST_DATA.at(y).size(); ++x) {
      costs[{y, x}] = TEST_DATA.at(y).at(x) != '0' ? 1 : 0;
      auto ij = std::array<TCODPATH_IndexType, 2>{y, x};
      REQUIRE(TCODPATH_map_get(costs.c_data(), ij.data()) == costs[{y, x}]);
    }
  };
  auto graph = as_2d_graph(costs, 1, 0);
  auto partition = Map2D(shape, -1);
  CHECK(TCODPATH_partition_from_graph(&graph, partition.c_data()) == 3);
  CHECK(as_string(partition) == as_string(TEST_DATA));
}

TEST_CASE("TCODPATH_partition large", "[.slow]") {
  auto costs = Map2D({2048, 2048}, 1);
  auto graph = as_2d_graph(costs, 1, 0);
  auto partition = Map2D(costs.get_shape(), 0);
  TCODPATH_partition_from_graph(&graph, partition.c_data());
}
