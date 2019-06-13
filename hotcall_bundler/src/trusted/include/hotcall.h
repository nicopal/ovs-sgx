#ifndef _H_LIB_HOTCALL_
#define _H_LIB_HOTCALL_

#include <sgx_spinlock.h>
#include <sgx_thread.h>
#include <stdbool.h>
#include <string.h>

#include "hotcall_do_while.h"
#include "hotcall_while.h"
#include "hotcall_for.h"
#include "hotcall_for_each.h"
#include "hotcall_filter.h"
#include "hotcall_map.h"
#include "hotcall_reduce.h"
#include "hotcall_if.h"
#include "hotcall_error.h"
#include "hotcall_predicate.h"

#define QUEUE_ITEM_TYPE_IF 0
#define QUEUE_ITEM_TYPE_IF_NULL 1
#define QUEUE_ITEM_TYPE_GUARD 2
#define QUEUE_ITEM_TYPE_DESTROY 3
#define QUEUE_ITEM_TYPE_FUNCTION 4
#define QUEUE_ITEM_TYPE_FOR_EACH 5
#define QUEUE_ITEM_TYPE_FOR_BEGIN 7
#define QUEUE_ITEM_TYPE_FILTER 8
#define QUEUE_ITEM_TYPE_DO_WHILE 9
#define QUEUE_ITEM_TYPE_WHILE_BEGIN 10
#define QUEUE_ITEM_TYPE_MAP 12
#define QUEUE_ITEM_TYPE_ERROR 13
#define QUEUE_ITEM_TYPE_REDUCE 14
#define QUEUE_ITEM_TYPE_LOOP_END 15
#define QUEUE_ITEM_TYPE_IF_ELSE 16

#define MAX_FCS 200
#define MAX_TS 200
#define MAX_N_VARIABLES 5

#define CHAIN(N, ARGS...) \
    hotcall_bundle_chain(N, ARGS)


#define CAT3(a, b, c) a ## b ## c
#define CAT2(a, b) a ## b
#define CAT(a, b, c) CAT3(a, b, c)
#define UNIQUE_ID CAT(_uid_, __LINE__,  __func__)

#define SWITCH_DEFAULT_REACHED printf("Default reached at %s %d\n", __FILE__, __LINE__);

#define RETURN  hotcall_bundle_error(_sm_ctx, 0)

#define PTR(...) (struct parameter) { .type = POINTER_TYPE_,   .value = { .pointer = { __VA_ARGS__ }}}
#define VAR(...) { .type = VARIABLE_TYPE_, .value = { .variable = { __VA_ARGS__ }}}
#define VECTOR(...) { .type = VECTOR_TYPE_, .value = { .vector = { __VA_ARGS__ }}}
#define FUNC(...) (struct parameter) { .type = FUNCTION_TYPE_, .value = { .function = { __VA_ARGS__ }}}

union hcall {
    struct hotcall_function fc;
    struct hotcall_if tif;
    struct hotcall_for_start for_s;
    struct hotcall_for_each tor;
    struct hotcall_filter fi;
    struct hotcall_do_while dw;
    struct hotcall_while_start while_s;
    struct hotcall_map ma;
    struct hotcall_error err;
    struct hotcall_reduce re;
    struct hotcall_function_ fc_;
};

struct ecall_queue_item {
    uint8_t type;
    union hcall call;
};

struct hotcall_batch {
    struct ecall_queue_item *queue[MAX_FCS];
    unsigned int queue_len;
    int error;
};

struct hotcall {
    sgx_thread_mutex_t mutex;
    sgx_spinlock_t spinlock;
    sgx_thread_cond_t cond;
    bool run;
    bool running_function;
    bool is_done;
    bool sleeping;
    int timeout_counter;
    bool hotcall_in_progress;
    bool is_inside_chain;
    struct hotcall_batch batch;
};

struct preallocated_function_calls {
    struct ecall_queue_item fcs[MAX_FCS];
    void *args[MAX_FCS][HOTCALL_MAX_ARG];
    size_t idx;
    size_t len;

    size_t idx_uint8;
    uint8_t uint8_ts[MAX_TS];

    size_t idx_sizet;
    size_t size_ts[MAX_TS];

    size_t idx_unsigned;
    unsigned int unsigned_ts[MAX_TS];

    size_t idx_void;
    void *void_ts[MAX_TS];

    size_t idx_uint32;
    uint32_t uint32_ts[MAX_TS];

    size_t idx_bool;
    bool bool_ts[MAX_TS];

    size_t idx_int;
    int int_ts[MAX_TS];
};

struct shared_memory_ctx {
  struct hotcall hcall;
  void *custom_object_ptr[MAX_N_VARIABLES];
  struct preallocated_function_calls pfc;
};

#endif
