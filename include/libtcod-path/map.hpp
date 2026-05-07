#pragma once

#include <libtcod-path/flow_tools.h>
#include <libtcod-path/graph_types.h>
#include <libtcod-path/map_types.h>

namespace tcod::path {
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
    return data_.at(ij.at(0) * shape_.at(0) + ij.at(1));
  }
  const value_type& operator[](const std::array<int, 2>& ij) const noexcept {
    range_check(ij);
    return data_.at(ij.at(0) * shape_.at(0) + ij.at(1));
  }

  TCODPATH_Map* c_data() noexcept { return &map_c_; }
  const TCODPATH_Map* c_data() const noexcept { return &map_c_; }

 private:
  TCODPATH_Map init_c_data() {
    TCODPATH_Map data{};
    data.contigious.type = TCODPATH_MAP_CONTIGIOUS;
    data.contigious.dimensions = 2;
    std::copy(shape_.begin(), shape_.end(), data.contigious.shape);
    data.contigious.int_type = static_cast<int>(sizeof(value_type)) * (std::is_signed_v<value_type> ? -1 : 1);
    data.contigious.data = reinterpret_cast<unsigned char*>(data_.data());
    return data;
  }

  std::array<index_type, 2> shape_{};
  std::vector<value_type> data_{};
  TCODPATH_Map map_c_{init_c_data()};
};
}  // namespace tcod::path
