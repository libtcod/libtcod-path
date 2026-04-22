#pragma once

#include "config.h"
#include "indexes.h"
#include "limits.h"
#include "map_types.h"
#include "utility.h"

/// @brief Return the dimensions of `map`. Returns `0` if invalid.
static inline int TCODPATH_map_get_dimensions(const TCODPATH_Map* __restrict map) {
  if (!map) return 0;
  switch (map->type) {
    case TCODPATH_MAP_CALLBACK:
      return map->callback.dimensions;
    case TCODPATH_MAP_CONTIGIOUS:
      return map->contigious.dimensions;
    case TCODPATH_MAP_STRIDES:
      return map->strides.dimensions;
    default:
      return 0;
  }
}
/// @brief Return the shape of `map`. Returns `NULL` if invalid.
static inline const TCODPATH_IndexType* TCODPATH_map_get_shape(const TCODPATH_Map* __restrict map) {
  if (!map) return NULL;
  switch (map->type) {
    case TCODPATH_MAP_CALLBACK:
      return map->callback.shape;
    case TCODPATH_MAP_CONTIGIOUS:
      return map->contigious.shape;
    case TCODPATH_MAP_STRIDES:
      return map->strides.shape;
    default:
      return NULL;
  }
}
/// @brief Returns true of `ij` is within the bounds of `map`.
/// @param map Pointer to map data. Can be `NULL`.
/// @param ij Node index. Array size must match the `map` dimensions. Can be `NULL`.
/// @return True of `ij` is within bounds, or false for out-of-bounds or invalid parameters.
static inline bool TCODPATH_map_in_bounds(const TCODPATH_Map* __restrict map, const TCODPATH_IndexType* __restrict ij) {
  if (!map || !ij) return false;  // Invalid map
  const TCODPATH_IndexType* bounds = TCODPATH_map_get_shape(map);
  if (!bounds) return true;  // Infinite bounds
  for (int i = 0; i < TCODPATH_map_get_dimensions(map); ++i) {
    if (ij[i] < 0 || ij[i] >= bounds[i]) return false;  // Out-of-bounds
  }
  return true;  // In-bounds
}
/// @brief Return the data at `ij` in `map`
/// @param map Pointer to a `map` with array data. Can be `NULL` or an incompatible map type.
/// @param ij Node index. Array size must match the `map` dimensions. Can be `NULL`.
/// @return A pointer to the data, or `NULL` for out-of-bounds or invalid parameters or incompatible map types.
static inline void* TCODPATH_map_at(TCODPATH_Map* __restrict map, const TCODPATH_IndexType* __restrict ij) {
  if (!map || !ij) return NULL;
  if (!TCODPATH_map_in_bounds(map, ij)) return NULL;
  switch (map->type) {
    case TCODPATH_MAP_CONTIGIOUS: {
      const unsigned char* at = map->contigious.data;
      ptrdiff_t stride = TCODPATH_ABS(map->contigious.int_size);
      for (int i = map->contigious.dimensions - 1; i >= 0; --i) {
        at += stride * ij[i];
        stride *= map->contigious.shape[i];
      }
      return (void*)at;
    }
    case TCODPATH_MAP_STRIDES: {
      const unsigned char* at = map->strides.data;
      for (int i = 0; i < map->strides.dimensions; ++i) at += map->strides.strides[i] * ij[i];
      return (void*)at;
    }
    default:
      return NULL;
  }
}
/// @brief Return the value at `ij` in `map`.
/// @param map Pointer to a `map`. Can be NULL.
/// @param ij Node index. Array size must match the `map` dimensions. Can be `NULL`.
/// @return The requested value, or `0` on invalid parameters.
static inline TCODPATH_ValueType TCODPATH_map_get(
    const TCODPATH_Map* __restrict map, const TCODPATH_IndexType* __restrict ij) {
  if (!map || !ij) return 0;
  switch (map->type) {
    case TCODPATH_MAP_CALLBACK:
      return map->callback.get(map->callback.userdata, ij);
    case TCODPATH_MAP_CONTIGIOUS:
    case TCODPATH_MAP_STRIDES: {
      const void* at = TCODPATH_map_at((TCODPATH_Map*)map, ij);
      if (at == NULL) return 0;  // Out-of-bounds
      switch (map->strides.int_size) {
        case 1:
          return (TCODPATH_ValueType)(*(uint8_t*)at);
        case 2:
          return (TCODPATH_ValueType)(*(uint16_t*)at);
        case 4:
          return (TCODPATH_ValueType)(*(uint32_t*)at);
        case 8:
          return (TCODPATH_ValueType)(*(uint64_t*)at);
        case -1:
          return (TCODPATH_ValueType)(*(int8_t*)at);
        case -2:
          return (TCODPATH_ValueType)(*(int16_t*)at);
        case -4:
          return (TCODPATH_ValueType)(*(int32_t*)at);
        case -8:
          return (TCODPATH_ValueType)(*(int64_t*)at);
        default:
          return 0;
      }
    }
    default:
      return 0;
  }
}
/// @brief Return true if the value at `ij` on `map` is the maximum value for `map`'s int type.
/// @param map Pointer to a `map`. Can be NULL.
/// @param ij Node index. Array size must match the `map` dimensions. Can be `NULL`.
/// @return True for confirmed maximum values, otherwise false.
static inline bool TCODPATH_map_is_max(const TCODPATH_Map* __restrict map, const TCODPATH_IndexType* __restrict ij) {
  if (!map || !ij) return 0;

  switch (map->type) {
    case TCODPATH_MAP_CALLBACK:
      return map->callback.get(map->callback.userdata, ij) == INT_MAX;
    case TCODPATH_MAP_CONTIGIOUS:
    case TCODPATH_MAP_STRIDES: {
      const void* at = TCODPATH_map_at((TCODPATH_Map*)map, ij);
      if (at == NULL) return 0;  // Out-of-bounds
      switch (map->strides.int_size) {
        case 1:
          return *(uint8_t*)at == UINT8_MAX;
        case 2:
          return *(uint16_t*)at == UINT16_MAX;
        case 4:
          return *(uint32_t*)at == UINT32_MAX;
        case 8:
          return *(uint64_t*)at == UINT64_MAX;
        case -1:
          return *(int8_t*)at == INT8_MAX;
        case -2:
          return *(int16_t*)at == INT16_MAX;
        case -4:
          return *(int32_t*)at == INT32_MAX;
        case -8:
          return *(int64_t*)at == INT64_MAX;
        default:
          return 0;
      }
    }
    default:
      return 0;
  }
}
/// @brief Set the value at `ij` for `map` to `value`.
/// Invalid parameters or read-only maps are silently ignored.
/// @param map Pointer to a `map`. Can be NULL.
/// @param ij Node index. Array size must match the `map` dimensions. Can be `NULL`.
/// @param value Value to set.
static inline void TCODPATH_map_set(
    TCODPATH_Map* __restrict map, const TCODPATH_IndexType* __restrict ij, TCODPATH_ValueType value) {
  if (!map || !ij) return;
  switch (map->type) {
    case TCODPATH_MAP_CALLBACK:
      return map->callback.set(map->callback.userdata, ij, value);
    case TCODPATH_MAP_CONTIGIOUS:
    case TCODPATH_MAP_STRIDES: {
      void* at = TCODPATH_map_at(map, ij);
      if (at == NULL) return;  // Out-of-bounds
      switch (map->strides.int_size) {
        case 1:
          *(uint8_t*)at = (uint8_t)value;
          return;
        case 2:
          *(uint16_t*)at = (uint16_t)value;
          return;
        case 4:
          *(uint32_t*)at = (uint32_t)value;
          return;
        case 8:
          *(uint64_t*)at = (uint64_t)value;
          return;
        case -1:
          *(int8_t*)at = (int8_t)value;
          return;
        case -2:
          *(int16_t*)at = (int16_t)value;
          return;
        case -4:
          *(int32_t*)at = (int32_t)value;
          return;
        case -8:
          *(int64_t*)at = (int64_t)value;
          return;
        default:
          return;
      }
    }
    default:
      return;
  }
}
