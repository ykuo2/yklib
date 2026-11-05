//
//  callback.h
//  yklib
//  collection of callback functions
//  Created by Yu Kuo on 2026/7/16.
//


#ifndef YKLIB_CALLBACK_FUNCTION_H
#define YKLIB_CALLBACK_FUNCTION_H
/*
 * Callback types
 */
// 目前先選擇用 compare func
// typedef int yk_match_fn(const void *, const void *);    // match: 1, not match: 0;
typedef int yk_compare_fn(const void *a, const void *b);  // return >0 if a>b, <0 if a<b, 0 if equal
typedef void yk_visit_fn(const void *data);  // eg. for display
typedef void yk_destroy_fn(void *data);  // to distroy/free data

#endif // YKLIB_CALLBACK_FUNCTION_H
