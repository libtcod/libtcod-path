#pragma once

#include <libtcod-path/graph_types.h>
#include <libtcod-path/map_types.h>

template <typename ValueType = TCODPATH_ValueType, typename IndexType = TCODPATH_IndexType>
class Map2D {
 public:
  using value_type = ValueType;
  using index_type = IndexType;

  Map2D() = default;
  Map2D(std::array<index_type, 2> shape, value_type default_value = 0)
      : shape_{std::move(shape)}, data_(shape_.at(0) * shape_.at(1), default_value) {}
  template <typename IndexType>
  Map2D(const std::array<IndexType, 2>& shape, value_type default_value = 0)
      : Map2D{{static_cast<index_type>(shape.at(0)), static_cast<index_type>(shape.at(1))}, default_value} {}

  auto get_shape() const noexcept -> const std::array<index_type, 2>& { return shape_; }

  bool in_bounds(const std::array<int, 2>& ij) const noexcept {
    return 0 <= ij.at(0) && ij.at(0) < shape_.at(0) && 0 <= ij.at(1) && ij.at(1) < shape_.at(1);
  }
  void range_check(const std::array<int, 2>& ij) const {
    if (!in_bounds(ij)) throw std::out_of_range("index out of range of Map2D");
  }

  value_type& operator[](const std::array<int, 2>& ij) noexcept {
    range_check(ij);
    return data_.at(ij.at(0) * shape_.at(1) + ij.at(1));
  }
  const value_type& operator[](const std::array<int, 2>& ij) const noexcept {
    range_check(ij);
    return data_.at(ij.at(0) * shape_.at(1) + ij.at(1));
  }

  TCODPATH_Map* c_data() noexcept { return &map_c_; }
  const TCODPATH_Map* c_data() const noexcept { return &map_c_; }

 private:
  TCODPATH_Map init_c_data() {
    TCODPATH_Map data{};
    data.contigious.type = TCODPATH_MAP_CONTIGIOUS;
    data.contigious.dimensions = 2;
    data.contigious.shape = shape_.data();
    data.contigious.int_size = static_cast<int>(sizeof(value_type)) * (std::is_signed_v<value_type> ? -1 : 1);
    data.contigious.data = reinterpret_cast<unsigned char*>(data_.data());
    return data;
  }

  std::array<index_type, 2> shape_{};
  std::vector<value_type> data_{};
  TCODPATH_Map map_c_{init_c_data()};
};

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
