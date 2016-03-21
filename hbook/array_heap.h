
#ifndef __ARRAY_HEAP_H__
#define __ARRAY_HEAP_H__

/* From LWROC, later DRASI...  */

/* The heap is stored in an @array, and has @entries items, each of
 * type @item_t.  They are ordered by the @compare_less function.
 */

#define HEAP_MOVE_DOWN(item_t, array, entries, compare_less,		\
		       move_index) do {					\
    int __parent = (move_index);					\
    for ( ; ; ) {							\
      int __child1 = __parent * 2 + 1;					\
      int __child2 = __child1 + 1;					\
      int __move_to = __parent;						\
      item_t __tmp_item;						\
      if (__child1 >= (entries))					\
	break; /* Parent has no children. */				\
      if (compare_less(array[__child1], array[__move_to]))		\
	__move_to = __child1;						\
      if (__child2 < (entries) &&					\
	  compare_less(array[__child2], array[__move_to]))		\
	__move_to = __child2;						\
      if (__move_to == __parent)					\
	break; /* Parent is smaller than children. */			\
      __tmp_item = array[__parent];					\
      array[__parent] = array[__move_to];				\
      array[__move_to] = __tmp_item;					\
      __parent = __move_to;						\
    }									\
  } while (0)

#define HEAP_INSERT(item_t, array, entries, compare_less,		\
		    insert_item) do {					\
    int __insert_at = (entries)++;					\
    while (__insert_at > 0) {						\
      int __parent_at = (__insert_at - 1) / 2;				\
      if (compare_less(array[__parent_at],insert_item))			\
	break; /* Parent is already smaller. */				\
      array[__insert_at] = array[__parent_at];				\
      __insert_at = __parent_at;					\
    }									\
    array[__insert_at] = insert_item;					\
  } while (0)

#endif/*__ARRAY_HEAP_H__*/
