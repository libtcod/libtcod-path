#pragma once

#include <libtcod-path/map.hpp>

using tcod::path::Map2D;

inline auto wall_costs_from_test_data(const std::vector<std::string>& test_data) -> Map2D<TCODPATH_IndexType> {
  auto map = Map2D<TCODPATH_IndexType>{std::array{test_data.size(), test_data.at(0).size()}};
  for (TCODPATH_IndexType y = 0; y < test_data.size(); ++y) {
    for (TCODPATH_IndexType x = 0; x < test_data.at(y).size(); ++x) {
      map[{y, x}] = test_data.at(y).at(x) != '#';
    }
  }
  return map;
}

template <typename T = int>
inline auto as_2d_graph(Map2D<T>& map, TCODPATH_ValueType cardinal, TCODPATH_ValueType diagonal) -> TCODPATH_Graph {
  auto graph = TCODPATH_Graph{};
  graph.basic2d = TCODPATH_GraphBasic2D{
      TCODPATH_GRAPH_BASIC2D,
      map.c_data(),
      cardinal,
      diagonal,
  };
  return graph;
}

inline auto as_string(const std::vector<std::string>& input) -> std::string {
  auto string = std::string{};
  for (int y = 0; y < input.size(); ++y) {
    string.append(input.at(y));
    if (y != input.size() - 1) string.push_back('\n');
  }
  return string;
}
template <typename T = int>
inline auto as_string(const Map2D<T>& input) -> std::string {
  auto string = std::string{};
  for (int y = 0; y < input.get_shape().at(0); ++y) {
    for (int x = 0; x < input.get_shape().at(1); ++x) {
      string.push_back(input[{y, x}] == std::numeric_limits<T>::max() ? '#' : '0' + (char)input[{y, x}]);
    }
    if (y != input.get_shape().at(0) - 1) string.push_back('\n');
  }
  return string;
}

/// Return the path on a flow map, excluding the starting point.
inline auto get_path(TCODPATH_Map& flow_map, std::array<TCODPATH_IndexType, 2> index)
    -> std::vector<std::array<TCODPATH_IndexType, 2>> {
  auto path = std::vector<std::array<TCODPATH_IndexType, 2>>{};
  while (TCODPATH_flow_iter_next(&flow_map, index.data()) == 0) path.push_back(index);
  return path;
}
