#pragma once

#include "indexes.h"
#include "limits.h"
#include "map_types.h"
#include "utility.h"

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

static inline const int* TCODPATH_map_get_shape(const TCODPATH_Map* __restrict map) {
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

static inline bool TCODPATH_map_in_bounds(const TCODPATH_Map* __restrict map, const int* __restrict ij) {
  if (!map || !ij) return false;  // Invalid map
  const int* bounds = TCODPATH_map_get_shape(map);
  if (!bounds) return true;  // Infinite bounds
  for (int i = 0; i < TCODPATH_map_get_dimensions(map); ++i) {
    if (ij[i] < 0 || ij[i] >= bounds[i]) return false;  // Out-of-bounds
  }
  return true;  // In-bounds
}

static inline void* TCODPATH_map_at(TCODPATH_Map* __restrict map, const int* __restrict ij) {
  if (!map || !ij) return NULL;
  if (!TCODPATH_map_in_bounds(map, ij)) return NULL;
  switch (map->type) {
    case TCODPATH_MAP_CONTIGIOUS: {
      const unsigned char* at = map->contigious.data;
      ptrdiff_t stride = TCODPATH_ABS(map->contigious.int_size);
      for (int i = 0; i < map->contigious.dimensions; ++i) {
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

static inline int TCODPATH_map_get(const TCODPATH_Map* __restrict map, const int* __restrict ij) {
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
          return *(uint8_t*)at;
        case 2:
          return *(uint16_t*)at;
        case 4:
          return *(uint32_t*)at;
        case 8:
          return *(uint64_t*)at;
        case -1:
          return *(int8_t*)at;
        case -2:
          return *(int16_t*)at;
        case -4:
          return *(int32_t*)at;
        case -8:
          return *(int64_t*)at;
        default:
          return 0;
      }
    }
    default:
      return 0;
  }
}

static inline bool TCODPATH_map_is_max(const TCODPATH_Map* __restrict map, const int* __restrict ij) {
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

static inline void TCODPATH_map_set(TCODPATH_Map* __restrict map, const int* __restrict ij, int value) {
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
