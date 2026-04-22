
#include <libtcod-path/graph_types.h>
#include <libtcod-path/partition.h>

#include <array>
#include <catch2/catch_all.hpp>
#include <limits>
#include <stdexcept>

#include "common.h"

TEST_CASE("TCODPATH_partition", "") {
  static const auto TEST_DATA = std::vector<std::string>{
      "111#22#",
      "111#2#3",
      "111##33",
  };
  static const auto shape =
      std::array<int, 2>{static_cast<int>(TEST_DATA.size()), static_cast<int>(TEST_DATA.at(0).size())};
  auto costs = Map2D(shape);
  for (int y = 0; y < TEST_DATA.size(); ++y) {
    for (int x = 0; x < TEST_DATA.at(y).size(); ++x) {
      costs[{y, x}] = TEST_DATA.at(y).at(x) != '#' ? 1 : 0;
    }
  };
  auto graph = as_2d_graph(costs, 1, 0);
  auto partition = Map2D(shape, -1);
  TCODPATH_partition_from_graph(&graph, partition.c_data());
  for (int y = 0; y < TEST_DATA.size(); ++y) {
    auto line = std::string();
    for (int x = 0; x < TEST_DATA.at(y).size(); ++x) {
      line.push_back((partition[{y, x}] == 0) ? '#' : '0' + (char)partition[{y, x}]);
    }
    CHECK(TEST_DATA.at(y) == line);
  };
}

TEST_CASE("TCODPATH_partition large", "") {
  auto costs = Map2D({2048, 2048}, 1);
  auto graph = as_2d_graph(costs, 1, 0);
  auto partition = Map2D(costs.get_shape(), 0);
  TCODPATH_partition_from_graph(&graph, partition.c_data());
}
