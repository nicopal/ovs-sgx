#ifndef _H_HOTCALL_UNTRUSTED__
#define _H_HOTCALL_UNTRUSTED__

#include "stdbool.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <hotcall.h>
#include "sgx_eid.h"


#ifdef __cplusplus
extern "C" {
#endif


#ifdef BATCHING
#define ASYNC(SM_CTX, X) X || is_transaction_in_progress(&(SM_CTX)->hcall)
#else
#define ASYNC(SM_CTX, X) false || is_transaction_in_progress(&(SM_CTX)->hcall)
#endif

#define HCALL(SM_CTX, F, _ASYNC, RET, N_ARGS, ARGS) \
    (SM_CTX)->hcall.ecall_queue[(SM_CTX)->hcall.queue_length++] = \
        get_fcall(&(SM_CTX)->pfc, QUEUE_ITEM_TYPE_FUNCTION, hotcall_ ## F, RET, N_ARGS, ARGS); \
    if(ASYNC(SM_CTX, _ASYNC) != 1) { \
        make_hotcall(&(SM_CTX)->hcall); \
    }

#define HCALL_CONTROL(SM_CTX, TYPE, _ASYNC, N_ARGS, ARGS) \
    (SM_CTX)->hcall.ecall_queue[(SM_CTX)->hcall.queue_length++] = \
        get_fcall(&(SM_CTX)->pfc, QUEUE_ITEM_TYPE_ ## TYPE, 0, NULL, N_ARGS, ARGS); \
    if(ASYNC(SM_CTX, _ASYNC) != 1) { \
        make_hotcall(&(SM_CTX)->hcall); \
    }

#define IF(SM_CTX, ARGS) \
    hotcall_bundle_if_(SM_CTX, ARGS);
#define THEN(...) __VA_ARGS__
#define ELSE(...) __VA_ARGS__


/*#define FOR_EACH(SM_CTX, FUNCTION, ITERS, N_PARAMS, PARAMS, FMT) \
    hotcall_bundle_for_each(SM_CTX, hotcall_ ## FUNCTION, ITERS, N_PARAMS, PARAMS, FMT)*/

/*#define FILTER(SM_CTX, FUNCTION, N_PARAMS, PARAMS, ITERS, LEN_FILTERED, FILTERED_OUT, FMT) \
    hotcall_bundle_filter(SM_CTX, hotcall_ ## FUNCTION, N_PARAMS, PARAMS, ITERS, LEN_FILTERED, FILTERED_OUT, FMT)*/

#define FOR_EACH(SM_CTX, FUNCTION, ARGS) hotcall_bundle_for_each(SM_CTX, hotcall_ ## FUNCTION, ARGS)


#define FILTER(SM_CTX, FUNCTION, ARGS) \
    hotcall_bundle_filter(SM_CTX, hotcall_ ## FUNCTION, ARGS)

#define MAP(SM_CTX, FUNCTION, ARGS) \
    hotcall_bundle_map(SM_CTX, hotcall_ ## FUNCTION, ARGS)

#define DO_WHILE(SM_CTX, F, ARGS) \
    hotcall_bundle_do_while((SM_CTX), hotcall_ ## F, ARGS)

#define BEGIN_FOR(SM_CTX, ARGS) \
    hotcall_bundle_for_begin(SM_CTX, ARGS)
#define END_FOR(SM_CTX, ARGS) \
    hotcall_bundle_for_end(SM_CTX, ARGS)
#define BEGIN_WHILE(SM_CTX, ARGS) \
    hotcall_bundle_while_begin(SM_CTX, ARGS)
#define END_WHILE(SM_CTX, ARGS) \
    hotcall_bundle_while_end(SM_CTX, ARGS)

extern inline bool
is_transaction_in_progress(struct hotcall *hcall) {
    return hcall->transaction_in_progress;
}


bool
hotcall_test();
void
hotcall_init(struct shared_memory_ctx *sm_ctx, sgx_enclave_id_t _global_eid);
void
hotcall_bundle_flush(struct shared_memory_ctx *sm_ctx);
void
hotcall_bundle_expected_value(struct preallocated_function_calls *pfc, int expected, int error_code, bool has_else);
void
hotcall_destroy(struct shared_memory_ctx *sm_ctx);


extern inline void
hotcall_bundle_begin(struct shared_memory_ctx *sm_ctx, int *transaction_error) {
    //transaction_err = transaction_error;
    sm_ctx->hcall.transaction_in_progress = true;
}

extern inline void
hotcall_bundle_end(struct shared_memory_ctx *sm_ctx) {
    hotcall_bundle_flush(sm_ctx);
    //transaction_err = NULL;
    sm_ctx->hcall.transaction_in_progress = false;
}


extern inline
struct ecall_queue_item * get_fcall(struct preallocated_function_calls *pfc, uint8_t f_type, uint8_t f_id, void *ret, uint8_t n_args, void **args) {
    struct ecall_queue_item *item = &pfc->fcs[pfc->idx++];
    item->type = f_type;
    switch(item->type) {
        case QUEUE_ITEM_TYPE_IF:
        {
            struct transaction_if *trans_if;
            trans_if = &item->call.tif;
            break;
        }
        case QUEUE_ITEM_TYPE_DESTROY:
            break;
        case QUEUE_ITEM_TYPE_FUNCTION:
        {
            struct function_call *fcall;
            fcall = &item->call.fc;
            fcall->id = f_id;
            fcall->args.n_args = n_args;
            fcall->return_value = ret;
            memcpy(fcall->args.args, args, fcall->args.n_args * sizeof(void *));
            break;
        }
        default:
            printf("unknown queue type\n");
    }
    if(pfc->idx == MAX_TS) {
        pfc->idx = 0;
    }
    return item;
}

extern inline void
make_hotcall(struct hotcall *hcall) {
    hcall->is_done  = false;
    hcall->run      = true;
    while (1) {
        sgx_spin_lock(&hcall->spinlock);
        if (hcall->is_done) {
            sgx_spin_unlock(&hcall->spinlock);
            break;
        }
        sgx_spin_unlock(&hcall->spinlock);
        for (int i = 0; i < 3; ++i) {
            __asm
            __volatile(
              "pause"
            );
        }
    }
}

extern inline size_t *
next_sizet(struct preallocated_function_calls *pfc, size_t x) {
    if(pfc->idx_sizet == MAX_TS) {
        pfc->idx_sizet = 0;
    }
    pfc->size_ts[pfc->idx_sizet] = x;
    return &pfc->size_ts[pfc->idx_sizet++];
}

extern inline uint8_t *
next_uint8(struct preallocated_function_calls *pfc, uint8_t x) {
    if(pfc->idx_uint8 == MAX_TS) {
        pfc->idx_uint8 = 0;
    }
    pfc->uint8_ts[pfc->idx_uint8] = x;
    return &pfc->uint8_ts[pfc->idx_uint8++];
}

extern inline unsigned int *
next_unsigned(struct preallocated_function_calls *pfc, unsigned int x) {
    if(pfc->idx_unsigned == MAX_TS) {
        pfc->idx_unsigned = 0;
    }
    pfc->unsigned_ts[pfc->idx_unsigned] = x;
    return &pfc->unsigned_ts[pfc->idx_unsigned++];
}

extern inline void *
next_voidptr(struct preallocated_function_calls *pfc, void *ptr) {
    if(pfc->idx_void == MAX_TS) {
        pfc->idx_void = 0;
    }
    pfc->void_ts[pfc->idx_void] = ptr;
    return pfc->void_ts[pfc->idx_void++];
}

extern inline void *
next_uint32(struct preallocated_function_calls *pfc, uint32_t x) {
    if(pfc->idx_uint32 == MAX_TS) {
        pfc->idx_uint32 = 0;
    }
    pfc->uint32_ts[pfc->idx_uint32] = x;
    return &pfc->uint32_ts[pfc->idx_uint32++];
}

extern inline void *
next_bool(struct preallocated_function_calls *pfc, bool b) {
    if(pfc->idx_bool == MAX_TS) {
        pfc->idx_bool = 0;
    }
    pfc->bool_ts[pfc->idx_bool] = b;
    return &pfc->bool_ts[pfc->idx_bool++];
}

extern inline void *
next_int(struct preallocated_function_calls *pfc, int x) {
    if(pfc->idx_int == MAX_TS) {
        pfc->idx_int = 0;
    }
    pfc->int_ts[pfc->idx_int] = x;
    return &pfc->int_ts[pfc->idx_int++];
}


extern inline void
hotcall_bundle_if_(struct shared_memory_ctx *sm_ctx, struct if_args *args) {
    struct ecall_queue_item *item;
    item = &sm_ctx->pfc.fcs[sm_ctx->pfc.idx++];
    item->type = QUEUE_ITEM_TYPE_IF;
    if(sm_ctx->pfc.idx == MAX_TS) {
        sm_ctx->pfc.idx = 0;
    }
    struct transaction_if *tif;
    tif = &item->call.tif;
    tif->args = args;
    sm_ctx->hcall.ecall_queue[sm_ctx->hcall.queue_length++] = item;
}

extern inline void
hotcall_bundle_filter(struct shared_memory_ctx *sm_ctx, uint8_t f, struct filter_args *args) {
    struct ecall_queue_item *item;
    item = &sm_ctx->pfc.fcs[sm_ctx->pfc.idx++];
    item->type = QUEUE_ITEM_TYPE_FILTER;
    if(sm_ctx->pfc.idx == MAX_TS) {
        sm_ctx->pfc.idx = 0;
    }
    struct transaction_filter *fi;
    fi = &item->call.fi;
    fi->f = f;
    fi->args = args;
    sm_ctx->hcall.ecall_queue[sm_ctx->hcall.queue_length++] = item;
}

extern inline void
hotcall_bundle_map(struct shared_memory_ctx *sm_ctx, uint8_t f, struct map_args *args) {
    struct ecall_queue_item *item;
    item = &sm_ctx->pfc.fcs[sm_ctx->pfc.idx++];
    item->type = QUEUE_ITEM_TYPE_MAP;
    if(sm_ctx->pfc.idx == MAX_TS) {
        sm_ctx->pfc.idx = 0;
    }
    struct transaction_map *ma;
    ma = &item->call.ma;
    ma->f = f;
    ma->args = args;
    sm_ctx->hcall.ecall_queue[sm_ctx->hcall.queue_length++] = item;
}

extern inline void
hotcall_bundle_do_while(struct shared_memory_ctx *sm_ctx, uint8_t f, struct do_while_args *do_while_args) {
    struct ecall_queue_item *item;
    item = &sm_ctx->pfc.fcs[sm_ctx->pfc.idx++];
    item->type = QUEUE_ITEM_TYPE_DO_WHILE;
    if(sm_ctx->pfc.idx == MAX_TS) {
        sm_ctx->pfc.idx = 0;
    }
    struct transaction_do_while *dw;
    dw = &item->call.dw;
    dw->f = f;
    dw->args = do_while_args;
    sm_ctx->hcall.ecall_queue[sm_ctx->hcall.queue_length++] = item;
}

extern inline void
hotcall_bundle_for_each(struct shared_memory_ctx *sm_ctx, uint8_t f, struct for_each_args *args) {
    struct ecall_queue_item *item;
    item = &sm_ctx->pfc.fcs[sm_ctx->pfc.idx++];
    item->type = QUEUE_ITEM_TYPE_FOR_EACH;
    if(sm_ctx->pfc.idx == MAX_TS) {
        sm_ctx->pfc.idx = 0;
    }
    struct transaction_for_each *tor;
    tor = &item->call.tor;
    tor->args = args;
    tor->f = f;
    sm_ctx->hcall.ecall_queue[sm_ctx->hcall.queue_length++] = item;
}

extern inline void
hotcall_bundle_for_begin(struct shared_memory_ctx *sm_ctx, struct for_args *args) {
    struct ecall_queue_item *item;
    item = &sm_ctx->pfc.fcs[sm_ctx->pfc.idx++];
    item->type = QUEUE_ITEM_TYPE_FOR_BEGIN;
    if(sm_ctx->pfc.idx == MAX_TS) {
        sm_ctx->pfc.idx = 0;
    }
    struct transaction_for_start *for_s;
    for_s = &item->call.for_s;
    for_s->args = args;
    sm_ctx->hcall.ecall_queue[sm_ctx->hcall.queue_length++] = item;
}

extern inline void
hotcall_bundle_while_begin(struct shared_memory_ctx *sm_ctx, struct while_args *args) {
    struct ecall_queue_item *item;
    item = &sm_ctx->pfc.fcs[sm_ctx->pfc.idx++];
    item->type = QUEUE_ITEM_TYPE_WHILE_BEGIN;
    if(sm_ctx->pfc.idx == MAX_TS) {
        sm_ctx->pfc.idx = 0;
    }
    struct transaction_while_start *while_s;
    while_s = &item->call.while_s;
    while_s->args = args;
    sm_ctx->hcall.ecall_queue[sm_ctx->hcall.queue_length++] = item;
}

extern inline void
hotcall_bundle_for_end(struct shared_memory_ctx *sm_ctx, struct for_args *args) {
    struct ecall_queue_item *item;
    item = &sm_ctx->pfc.fcs[sm_ctx->pfc.idx++];
    item->type = QUEUE_ITEM_TYPE_FOR_END;
    if(sm_ctx->pfc.idx == MAX_TS) {
        sm_ctx->pfc.idx = 0;
    }
    struct transaction_for_end *for_e;
    for_e = &item->call.for_e;
    for_e->args = args;
    sm_ctx->hcall.ecall_queue[sm_ctx->hcall.queue_length++] = item;
}

extern inline void
hotcall_bundle_while_end(struct shared_memory_ctx *sm_ctx, struct while_args *args) {
    struct ecall_queue_item *item;
    item = &sm_ctx->pfc.fcs[sm_ctx->pfc.idx++];
    item->type = QUEUE_ITEM_TYPE_WHILE_END;
    if(sm_ctx->pfc.idx == MAX_TS) {
        sm_ctx->pfc.idx = 0;
    }
    struct transaction_while_end *while_e;
    while_e = &item->call.while_e;
    while_e->args = args;
    sm_ctx->hcall.ecall_queue[sm_ctx->hcall.queue_length++] = item;
}



#ifdef __cplusplus
}
#endif

#endif
