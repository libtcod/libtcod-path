#pragma once

#include <libtcod-path/graph_types.h>
#include <libtcod-path/map_types.h>

template <typename T = int>
class Map2D {
 public:
  Map2D() = default;
  Map2D(std::array<int, 2> shape, T default_value = 0)
      : shape_{std::move(shape)}, data_(shape_.at(0) * shape_.at(1), default_value) {}

  bool in_bounds(const std::array<int, 2>& ij) const noexcept {
    return 0 <= ij.at(0) && ij.at(0) < shape_.at(0) && 0 <= ij.at(1) && ij.at(1) < shape_.at(1);
  }
  void range_check(const std::array<int, 2>& ij) const {
    if (!in_bounds(ij)) throw std::out_of_range("index out of range of Map2D");
  }

  T& operator[](const std::array<int, 2>& ij) noexcept {
    range_check(ij);
    return data_.at(ij.at(0) * shape_.at(1) + ij.at(1));
  }
  const T& operator[](const std::array<int, 2>& ij) const noexcept {
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
    data.contigious.int_size = -4;
    data.contigious.data = reinterpret_cast<unsigned char*>(data_.data());
    return data;
  }

  std::array<int, 2> shape_{};
  std::vector<T> data_{};
  TCODPATH_Map map_c_{init_c_data()};
};

template <typename T = int>
auto as_2d_graph(Map2D<T>& map, int cardinal, int diagonal) -> TCODPATH_Graph {
  auto graph = TCODPATH_Graph{};
  graph.basic2d = TCODPATH_GraphBasic2D{
      TCODPATH_GRAPH_BASIC2D,
      map.c_data(),
      cardinal,
      diagonal,
  };
  return graph;
}
