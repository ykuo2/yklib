//
//  sort.h
//  yklib
//
//  Created by Yu Kuo on 2026/8/14.
//

#ifndef YKLIB_SORT_H
#define YKLIB_SORT_H

#include <yklib/core/callback.h>
#include <stddef.h>

void yk_quick_sort(void *base, size_t nmeb, size_t size, yk_compare_fn compare);

#endif  // YKLIB_SORT_H



