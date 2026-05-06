#pragma once
#include <assert.h>

#include "config.h"
#include "uniform_cost_search.h"

/// @brief Narrow the map `differentials` into a single index on `out`.
/// @param differentials Map of `differentials`
/// @param differential_index Index to select
/// @param out Output map which references only `differential_index` on `differentials`
static inline void TCODPATH_differential_map_slice(
    TCODPATH_Map* __restrict differentials, int differential_index, TCODPATH_Map* __restrict out) {
  if (!differentials || !out) return;
  switch (differentials->type) {
    case TCODPATH_MAP_CONTIGIOUS: {
      // Convert contiguous array into the equivalent strides, then call into the TCODPATH_MAP_STRIDES branch
      TCODPATH_Map as_strides;
      as_strides.strides = TCODPATH_MapStrides{TCODPATH_MAP_STRIDES};
      as_strides.strides.dimensions = differentials->contigious.dimensions;
      memcpy(as_strides.strides.shape, differentials->contigious.shape, sizeof(as_strides.strides.shape));
      as_strides.strides.int_type = differentials->contigious.int_type;
      as_strides.strides.data = differentials->contigious.data;
      return TCODPATH_differential_map_slice(&as_strides, differential_index, out);
    }
    case TCODPATH_MAP_STRIDES:
      // Drop the last axis then shift data to the selected differential_index
      out->strides = differentials->strides;
      out->strides.dimensions -= 1;
      out->strides.data += differentials->strides.strides[out->strides.dimensions] * differential_index;
      return;
    default:
      assert(0);
      return;
  }
}
/// @brief Generate differentials for `differential_index` using the provided pivot indexes.
static inline void TCODPATH_differential_generate_one(
    TCODPATH_Graph* __restrict graph,
    TCODPATH_Map* __restrict differentials,
    int differential_index,
    int pivot_n,
    TCODPATH_IndexType* __restrict pivot_ij) {
  assert(graph);
  assert(differentials);
  assert(pivot_ij);
  TCODPATH_Map differential_slice;
  TCODPATH_differential_map_slice(differentials, differential_index, &differential_slice);
  TCODPATH_map_clear_max(&differential_slice);
  while (pivot_n--) {
    TCODPATH_map_set(&differential_slice, pivot_ij, 0);
    pivot_ij += TCODPATH_map_get_dimensions(&differential_slice);
  }
  TCODPATH_dijkstra(graph, &differential_slice, NULL);
}
/// @brief Generate differentials for `differential_index` automatically.
static inline void TCODPATH_differential_generate_one_auto(
    TCODPATH_Graph* __restrict graph,
    int partition_count,
    TCODPATH_Map* __restrict partition,
    TCODPATH_Map* __restrict differentials,
    int differential_index,
    int start_index,
    int end_index) {
  assert(graph);
  assert(partition);
  assert(differentials);
  TCODPATH_Map differential_slice;
  TCODPATH_differential_map_slice(differentials, differential_index, &differential_slice);
  TCODPATH_map_clear_max(&differential_slice);
  TCODPATH_IndexType index[TCODPATH_MAX_DIMENSIONS];
  const int dimensions = TCODPATH_map_get_dimensions(&differential_slice);
  const bool no_differentials_exist = start_index == end_index;
  TCODPATH_IndexType* pivots_ij = (TCODPATH_IndexType*)calloc(dimensions * partition_count, sizeof(*pivots_ij));
  TCODPATH_ValueType* best_values = (TCODPATH_ValueType*)malloc(partition_count * sizeof(*best_values));
  for (int i = 0; i < partition_count; ++i) best_values[i] = TCODPATH_VALUE_MAX;
  for (TCODPATH_indexes_iter_begin(dimensions, index);
       TCODPATH_indexes_iter_step(dimensions, TCODPATH_map_get_shape(&differential_slice), index);) {
    const TCODPATH_ValueType this_partition = TCODPATH_map_get(partition, index) - 1;
    if (this_partition <= -1 || this_partition >= partition_count) continue;

    TCODPATH_IndexType differentials_index[TCODPATH_MAX_DIMENSIONS];
    for (int i = 0; i < dimensions; ++i) differentials_index[i] = index[i];
    TCODPATH_ValueType this_best_value = TCODPATH_VALUE_MAX;
    for (TCODPATH_ValueType i = start_index; i != end_index; ++i) {
      differentials_index[dimensions] = i;
      if (TCODPATH_map_is_max(differentials, differentials_index)) continue;
      const TCODPATH_ValueType this_value = TCODPATH_map_get(differentials, differentials_index);
      this_best_value = TCODPATH_MIN(this_best_value, this_value);
    }
    if (no_differentials_exist) this_best_value = TCODPATH_MIN(this_best_value, TCODPATH_VALUE_MAX - 1);
    if (this_best_value < best_values[this_partition]) {
      best_values[this_partition] = this_best_value;
      for (int i = 0; i < dimensions; ++i) pivots_ij[this_partition * dimensions + i] = index[i];
    }
  }
  free(best_values);
  TCODPATH_differential_generate_one(graph, differentials, differential_index, partition_count, pivots_ij);
  free(pivots_ij);
}

static inline void TCODPATH_differential_generate_all_auto(
    TCODPATH_Graph* __restrict graph,
    int partition_count,
    TCODPATH_Map* __restrict partition,
    TCODPATH_Map* __restrict differentials) {
  assert(graph);
  assert(partition);
  assert(differentials);
  const int differentials_count = TCODPATH_map_get_shape(differentials)[TCODPATH_map_get_dimensions(differentials) - 1];
  for (int end_index = 0; end_index < differentials_count; ++end_index) {
    TCODPATH_differential_generate_one_auto(graph, partition_count, partition, differentials, end_index, 0, end_index);
  }
}
