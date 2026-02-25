#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "config.h"

typedef enum TCODPATH_MapTypes {
  TCODPATH_MAP_UNDEFINED = 0,
  TCODPATH_MAP_CALLBACK = 1,
  TCODPATH_MAP_CONTIGIOUS = 2,
  TCODPATH_MAP_STRIDES = 3,
} TCODPATH_MapTypes;

struct TCODPATH_MapCallback {
  TCODPATH_MapTypes type;  // Must be TCODPATH_MAP_CALLBACK
  int dimensions;
  int* __restrict shape;
  void* userdata;
  int (*get)(void* userdata, const int* __restrict ij);  // Get callback
  void (*set)(void* userdata, const int* __restrict ij, int v);  // Set callback
};

struct TCODPATH_MapContigious {
  TCODPATH_MapTypes type;  // Must be TCODPATH_MAP_CONTIGIOUS
  int dimensions;
  int* __restrict shape;
  int8_t int_size;  // data array integer byte-size plus sign: -4=int32_t, 1=uint8_t, etc
  unsigned char* __restrict data;  // Pointer to contigious integer array
};

struct TCODPATH_MapStrides {
  TCODPATH_MapTypes type;  // Must be TCODPATH_MAP_STRIDES
  int dimensions;
  int* __restrict shape;
  int8_t int_size;  // data array integer byte-size plus sign: -4=int32_t, 1=uint8_t, etc
  unsigned char* __restrict data;  // Pointer to strided integer array
  ptrdiff_t* __restrict strides;  // Strides for each axis in bytes
};

/// @brief Union type for tile maps.
typedef union TCODPATH_Map {
  TCODPATH_MapTypes type;
  struct TCODPATH_MapCallback callback;
  struct TCODPATH_MapContigious contigious;
  struct TCODPATH_MapStrides strides;
} TCODPATH_Map;
