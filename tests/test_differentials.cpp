
#include <libtcod-path/differential.h>
#include <libtcod-path/partition.h>

#include <array>
#include <catch2/catch_all.hpp>
#include <limits>
#include <stdexcept>

#include "common.h"

TEST_CASE("TCODPATH_differentials", "") {
  auto differentials_data = std::vector<int>(5 * 5 * 4);
  auto differentials_shape = std::vector<int>{5, 5, 4};
  auto differentials = TCODPATH_Map{};
  differentials.contigious.type = TCODPATH_MAP_CONTIGIOUS;
  differentials.contigious.int_type = -4;
  differentials.contigious.dimensions = 3;
  differentials.contigious.data = (unsigned char*)differentials_data.data();
  differentials.contigious.shape = differentials_shape.data();
  auto partition = Map2D({5, 5}, 1);
  auto cost = Map2D({5, 5}, 1);

  auto graph = as_2d_graph(cost, 1, 1);

  TCODPATH_differential_generate_all_auto(&graph, 1, partition.c_data(), &differentials);
}
