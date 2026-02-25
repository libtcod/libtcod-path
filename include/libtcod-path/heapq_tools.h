
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "heapq_types.h"

#define TCODPATH_set_errorv(x) -1
#define TCODPATH_set_errorvf(x, ...) -1

#define TCODPATH_HEAP_DEFAULT_CAPACITY 256
#define TCODPATH_HEAP_MAX_NODE_SIZE 256
/***************************************************************************
    @brief Clear a heap and free its data.

    @param heap A pointer to a TCODPATH_Heap struct, the struct itself is not freed.
 */
void TCODPATH_heap_uninit(struct TCODPATH_Heap* heap) {
  if (heap->heap) free(heap->heap);
  heap->heap = NULL;
  heap->size = 0;
  heap->capacity = 0;
  heap->node_size = 0;
  heap->data_size = 0;
  heap->data_offset = 0;
}
/***************************************************************************
    @brief Initialize a heap with the given data_size.

    @param heap A pointer to an existing TCODPATH_Heap struct.
    @param data_size The size of the user data in bytes.
    @return int Returns a negative value on error.
 */
int TCODPATH_heap_init(struct TCODPATH_Heap* heap, size_t data_size) {
  size_t node_size = sizeof(int) + data_size;
  if (node_size > TCODPATH_HEAP_MAX_NODE_SIZE) {
    return TCODPATH_set_errorvf("Heap data size is too large: %i", (int)node_size);
  }
  heap->heap = NULL;
  heap->size = 0;
  heap->capacity = 0;
  heap->node_size = node_size;
  heap->data_size = data_size;
  heap->data_offset = sizeof(int);
  heap->priority_type = -4;  // Signed int type.
  return 0;
}
/***************************************************************************
    @brief Clear all elements from this heap.

    @param heap A TCODPATH_Heap pointer.
 */
void TCODPATH_heap_clear(struct TCODPATH_Heap* heap) { heap->size = 0; }
/// Return a pointer to the node at index in heap.  This points directly to the priority value.
/// Used internally.
static void* TCODPATH_heap_get_(struct TCODPATH_Heap* heap, int index) {
  return (void*)(heap->heap + index * heap->node_size);
}
/// Return a pointer to user data of the node at index in heap.
/// Used internally.
static void* TCODPATH_heap_address_data_(struct TCODPATH_Heap* heap, int index) {
  return heap->heap + index * heap->node_size + heap->data_offset;
}
/// Swap two heap elements with each other.
/// Used internally.
static void TCODPATH_heap_swap_(struct TCODPATH_Heap* heap, int lhs, int rhs) {
  unsigned char buffer[TCODPATH_HEAP_MAX_NODE_SIZE];
  memcpy(buffer, TCODPATH_heap_get_(heap, lhs), heap->node_size);
  memcpy(TCODPATH_heap_get_(heap, lhs), TCODPATH_heap_get_(heap, rhs), heap->node_size);
  memcpy(TCODPATH_heap_get_(heap, rhs), buffer, heap->node_size);
}
/// Manually set the priority and data of an existing heap element.
/// Used internally.
static void TCODPATH_heap_set_(
    struct TCODPATH_Heap* __restrict heap, int index, int priority, const void* __restrict data) {
  assert(heap->priority_type == -4);
  int* node_priority = (int*)TCODPATH_heap_get_(heap, index);
  void* node_data = TCODPATH_heap_address_data_(heap, index);
  *node_priority = priority;
  memcpy(node_data, data, heap->data_size);
}
/// Copy a heap element from the index src to the index dest.
/// Used internally.
static void TCODPATH_heap_copy_(struct TCODPATH_Heap* heap, int dest, int src) {
  memcpy(TCODPATH_heap_get_(heap, dest), TCODPATH_heap_get_(heap, src), heap->node_size);
}
/// Returns true if the priority of the left index is less than the right index.
/// Used internally.
static bool TCODPATH_minheap_compare_(struct TCODPATH_Heap* minheap, int lhs_index, int rhs_index) {
  assert(minheap->priority_type == -4);
  const int* lhs_priority = (int*)TCODPATH_heap_get_(minheap, lhs_index);
  const int* rhs_priority = (int*)TCODPATH_heap_get_(minheap, rhs_index);
  return *lhs_priority < *rhs_priority;
}
/// Sort a heap element downwards, away from the root.
/// Used internally.
static void TCODPATH_minheap_heapify_down_(struct TCODPATH_Heap* minheap, int index) {
  int candidate = index;
  const int left = index * 2 + 1;
  const int right = index * 2 + 2;
  if (left < minheap->size && TCODPATH_minheap_compare_(minheap, left, candidate)) candidate = left;
  if (right < minheap->size && TCODPATH_minheap_compare_(minheap, right, candidate)) candidate = right;
  if (candidate != index) {
    TCODPATH_heap_swap_(minheap, index, candidate);
    TCODPATH_minheap_heapify_down_(minheap, candidate);
  }
}
/// Sort a heap element upwards, towards from the root.
/// Used internally.
static void TCODPATH_minheap_heapify_up_(struct TCODPATH_Heap* minheap, int index) {
  if (index == 0) return;  // No element is higher than the current one.
  const int parent = (index - 1) >> 1;
  if (TCODPATH_minheap_compare_(minheap, index, parent)) {
    TCODPATH_heap_swap_(minheap, index, parent);
    TCODPATH_minheap_heapify_up_(minheap, parent);
  }
}
/***************************************************************************
    @brief Sort the heap elements into a valid heap.

    @param minheap A TCODPATH_Heap pointer.
 */
void TCODPATH_minheap_heapify(struct TCODPATH_Heap* minheap) {
  for (int i = minheap->size / 2; i >= 0; --i) TCODPATH_minheap_heapify_down_(minheap, i);
}
/***************************************************************************
    @brief Remove the smallest element from the heap and keep it sorted.

    @param minheap A TCODPATH_Heap pointer.
    @param out An optional pointer to store the data from the removed element.
 */
void TCODPATH_minheap_pop(struct TCODPATH_Heap* __restrict minheap, void* __restrict out) {
  if (minheap->size == 0) return;  // No element to pop.
  if (out) memcpy(out, minheap->heap + minheap->data_offset, minheap->data_size);
  TCODPATH_heap_copy_(minheap, 0, minheap->size - 1);
  --minheap->size;
  TCODPATH_minheap_heapify_down_(minheap, 0);
}
/***************************************************************************
    @brief Push an element onto this minumum heap.

    @param minheap A TCODPATH_Heap pointer.
    @param priority The priority of the new element.
    @param data The data to push onto the heap.  Can not be NULL.
    @return Returns a negative error code on failures.
 */
int TCODPATH_minheap_push(struct TCODPATH_Heap* __restrict minheap, int priority, const void* __restrict data) {
  if (minheap->size == minheap->capacity) {
    const int new_capacity = (minheap->capacity ? minheap->capacity * 2 : TCODPATH_HEAP_DEFAULT_CAPACITY);
    void* new_heap = realloc(minheap->heap, minheap->node_size * new_capacity);
    if (!new_heap) {
      TCODPATH_set_errorv("Out of memory while reallocating heap.");
      return TCODPATH_E_OUT_OF_MEMORY;
    }
    minheap->capacity = new_capacity;
    minheap->heap = (unsigned char*)new_heap;
  }
  ++minheap->size;
  TCODPATH_heap_set_(minheap, minheap->size - 1, priority, data);
  TCODPATH_minheap_heapify_up_(minheap, minheap->size - 1);
  return TCODPATH_E_OK;
}
