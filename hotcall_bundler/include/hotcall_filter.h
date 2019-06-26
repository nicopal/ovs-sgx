#ifndef _H_HOTCALL_FILTER_
#define _H_HOTCALL_FILTER_

#include "hotcall_utils.h"
#include "hotcall_function.h"

#define _FILTER(SM_CTX, ID, CONFIG, ...) \
    struct parameter CAT2(FILTER_ARG_,ID[]) = { \
        __VA_ARGS__\
    }; \
    struct filter_config CAT2(FILTER_CONFIG_,ID) = CONFIG;\
    struct postfix_item CAT2(POSTFIX_, ID)[strlen(CAT2(FILTER_CONFIG_, ID).predicate_fmt)];\
    CAT2(FILTER_CONFIG_,ID).postfix = CAT2(POSTFIX_, ID);\
    if(!(SM_CTX)->hcall.batch.ignore_hcalls) { \
        CAT2(FILTER_CONFIG_,ID).n_params = sizeof(CAT2(FILTER_ARG_,ID))/sizeof(struct parameter);\
        hotcall_enqueue_item(SM_CTX, QUEUE_ITEM_TYPE_FILTER, &CAT2(FILTER_CONFIG_,ID), CAT2(FILTER_ARG_, ID));\
    }

#define FILTER(CONFIG, ...) \
    _FILTER(_sm_ctx, UNIQUE_ID, CONFIG, __VA_ARGS__)


struct filter_config {
    const char *predicate_fmt;
    struct parameter *input_vector;
    uint8_t n_params;
    struct postfix_item *postfix;
    unsigned int postfix_length;
};

struct hotcall_filter {
    struct parameter *params;
    struct filter_config *config;
};

#endif