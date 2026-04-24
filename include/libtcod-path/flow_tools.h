#pragma once
#include "map_tools.h"

/// @brief Reset a flow map. All index values will refer to their own index.
/// @param flow_map Pointer to a flow map.
static inline void TCODPATH_flow_reset(TCODPATH_Map* __restrict flow_map) {
  if (!flow_map) return;
  const int dimensions = TCODPATH_map_get_dimensions(flow_map);
  const TCODPATH_IndexType* __restrict shape = TCODPATH_map_get_shape(flow_map);
  if (dimensions <= 0 || !shape) return;
  TCODPATH_IndexType index[TCODPATH_MAX_DIMENSIONS];
  for (TCODPATH_indexes_iter_begin(dimensions - 1, index); TCODPATH_indexes_iter_step(dimensions - 1, shape, index);) {
    TCODPATH_map_set_index(flow_map, index, index);
  }
}
/// @brief Replace `ij` with the next index in this flow map. Return 1 when end is reached.
/// @param flow_map Pointer to a flow map.
/// @param ij Current position on the flow map.
/// @return 0 if `ij` was changed. 1 if end was reached. Note that flow maps may be cyclic.
static inline int TCODPATH_flow_iter_next(const TCODPATH_Map* __restrict flow_map, TCODPATH_IndexType* __restrict ij) {
  if (!flow_map) return -1;
  const int dimensions = TCODPATH_map_get_dimensions(flow_map);
  TCODPATH_IndexType prev[TCODPATH_MAX_DIMENSIONS - 1];
  for (int i = 0; i < dimensions - 1; ++i) prev[i] = ij[i];
  TCODPATH_map_get_index(flow_map, ij, ij);
  for (int i = 0; i < dimensions - 1; ++i) {
    if (ij[i] != prev[i]) return 0;  // Index has changed
  }
  return 1;  // End has been reached
}
