/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_LIST_SORT_H
#define _LINUX_LIST_SORT_H

// #include <linux/types.h>
#include "list.h"
#include "queue.h"

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

struct list_head;
typedef unsigned char u8;
typedef int
    __attribute__((nonnull(2, 3))) (*list_cmp_func_t)(void *,
                                                      const struct list_head *,
                                                      const struct list_head *);

__attribute__((nonnull(2, 3))) void list_sort(void *priv,
                                              struct list_head *head,
                                              list_cmp_func_t cmp);
#endif