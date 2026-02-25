
#include <libtcod-path/map_tools.h>
#include <libtcod-path/map_types.h>

#include <array>
#include <catch2/catch_all.hpp>

TEST_CASE("TCODPATH_MapContigious", "") {
  auto data = std::array<int, 16 * 24>{};
  auto shape = std::array{16, 24};
  auto contigious = TCODPATH_Map{};
  contigious.contigious = TCODPATH_MapContigious{
      TCODPATH_MAP_CONTIGIOUS,
      std::ssize(shape),
      shape.data(),
      -4,
      reinterpret_cast<unsigned char*>(data.data()),
  };
  REQUIRE(TCODPATH_map_get_dimensions(&contigious) == shape.size());
  REQUIRE(TCODPATH_map_get(&contigious, std::array{0, 0}.data()) == 0);
  TCODPATH_map_set(&contigious, std::array{0, 0}.data(), 1);
  REQUIRE(TCODPATH_map_get(&contigious, std::array{0, 0}.data()) == 1);
  TCODPATH_map_set(&contigious, std::array{-1, -1}.data(), 1);
  REQUIRE(TCODPATH_map_get(&contigious, std::array{-1, -1}.data()) == 0);
}
