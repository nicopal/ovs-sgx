#include "gtest/gtest.h"
#include "test.h"
#include "hotcall-bundler-untrusted.h"
#include "functions.h"

#include "hotcall_filter.h"

TEST(filter,1) {
    //Contract: Filter should filter out all elements with a value less than 3,

    hotcall_test_setup();

    BUNDLE_BEGIN();

    unsigned int n_iters = 10, out_length;
    int xs[n_iters] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int ys[n_iters] = { 0 };

    struct parameter vec1[] = { VAR(xs, 'd'), VECTOR_v2(&vec1[0], &n_iters) }, p1 = vec1[1];
    struct parameter vec2[] = { VAR(ys, 'd'), VECTOR_v2(&vec2[0], &out_length) }, p2 = vec2[1];

    struct parameter function_parameter[] = {
        p1
    };

    FILTER(
        ((struct filter_config) { .condition_fmt = "b" }),
        FUNC(.function_id = hotcall_ecall_greater_than_two, .params = function_parameter, .n_params = 1),
        p2
    );

    BUNDLE_END();

    hotcall_test_teardown();

    // check that orignal list is unmodified.
    for(int i = 0; i < n_iters; ++i) {
        ASSERT_EQ(xs[i], i);
    }

    ASSERT_EQ(out_length, 7);
    for(int i = 0; i < out_length; ++i) {
        ASSERT_EQ(ys[i], i + 3);
    }
}


TEST(filter,2) {
    //Contract: Filter should filter out all elements with a value less than 7,

    hotcall_test_setup();

    unsigned int n_iters = 10, out_length;
    int xs[n_iters] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int ys[n_iters] = { 0 };
    int y = 6;

    struct parameter vec1[] = { VAR(xs, 'd'), VECTOR_v2(&vec1[0], &n_iters) }, p1 = vec1[1];
    struct parameter vec2[] = { VAR(ys, 'd'), VECTOR_v2(&vec2[0], &out_length) }, p2 = vec2[1];

    BUNDLE_BEGIN();

    FILTER(
        ((struct filter_config) { .condition_fmt = "d>d" }),
        p1, VAR(y, 'd'), p2
    );

    BUNDLE_END();

    hotcall_test_teardown();

    // check that orignal list is unmodified.
    for(int i = 0; i < n_iters; ++i) {
        ASSERT_EQ(xs[i], i);
    }
    ASSERT_EQ(out_length, 3);
    for(int i = 0; i < out_length; ++i) {
        ASSERT_EQ(ys[i], i + 7);
    }
}


TEST(filter,3) {
    //Contract: Filter should filter out all elements with a value less than 7,

    hotcall_test_setup();

    BUNDLE_BEGIN();

    unsigned int n_iters = 10, out_length;
    int xs[n_iters] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int ys[n_iters] = { 0 };
    int y = 6;

    struct parameter vec1[] = { VAR(xs, 'd'), VECTOR_v2(&vec1[0], &n_iters) }, p1 = vec1[1];
    struct parameter vec2[] = { VAR(ys, 'd'), VECTOR_v2(&vec2[0], &out_length) }, p2 = vec2[1];

    struct parameter function_parameters[] = { p1, VAR(y, 'd') };

    FILTER(
        ((struct filter_config) { .condition_fmt = "b" }),
        FUNC(.function_id = hotcall_ecall_greater_than_y, .params = function_parameters, .n_params = 2),
        p2
    );

    BUNDLE_END();

    hotcall_test_teardown();

    // check that orignal list is unmodified.
    for(int i = 0; i < n_iters; ++i) {
        ASSERT_EQ(xs[i], i);
    }
    ASSERT_EQ(out_length, 3);
    for(int i = 0; i < out_length; ++i) {
        ASSERT_EQ(ys[i], i + 7);
    }
}

TEST(filter,4) {
    //Contract: Filter should filter out all elements with a value less than 7,

    hotcall_test_setup();

    BUNDLE_BEGIN();

    unsigned int n_iters = 10, out_length;
    unsigned xs[n_iters] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }, ys[n_iters] = { 0 };
    unsigned int y = 6;

    struct parameter vec1[] = { VAR(xs, 'u'), VECTOR_v2(&vec1[0], &n_iters) }, p1 = vec1[1];
    struct parameter vec2[] = { VAR(ys, 'u'), VECTOR_v2(&vec2[0], &out_length) }, p2 = vec2[1];


    FILTER(
        ((struct filter_config) { .condition_fmt = "u>u" }),
        p1, VAR(y, 'u'), p2
    );

    BUNDLE_END();

    hotcall_test_teardown();

    // check that orignal list is unmodified.
    for(int i = 0; i < n_iters; ++i) {
        ASSERT_EQ(xs[i], i);
    }

    ASSERT_EQ(out_length, 3);

    for(int i = 0; i < out_length; ++i) {
        ASSERT_EQ(ys[i], i + 7);
    }
}

TEST(filter,5) {
    //Contract: Filter should filter out all elements with a value less than 7,

    hotcall_test_setup();

    BUNDLE_BEGIN();

    unsigned int n_iters = 10, out_length;
    unsigned xs[n_iters] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }, ys[n_iters] = { 0 };
    unsigned int y = 6;

    struct parameter vec1[] = { VAR(xs, 'u'), VECTOR_v2(&vec1[0], &n_iters) }, p1 = vec1[1];
    struct parameter vec2[] = { VAR(ys, 'u'), VECTOR_v2(&vec2[0], &out_length) }, p2 = vec2[1];


    FILTER(
        ((struct filter_config) { .condition_fmt = "u<u" }),
        VAR(y, 'u'), p1, p2
    );

    BUNDLE_END();

    hotcall_test_teardown();

    // check that orignal list is unmodified.
    for(int i = 0; i < n_iters; ++i) {
        ASSERT_EQ(xs[i], i);
    }

    ASSERT_EQ(out_length, 3);

    for(int i = 0; i < out_length; ++i) {
        ASSERT_EQ(ys[i], i + 7);
    }
}

TEST(filter,6) {
    //Contract:

    hotcall_test_setup();

    BUNDLE_BEGIN();

    unsigned int n_iters = 10, out_length;
    unsigned xs[n_iters] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }, ys[n_iters] = { 0, 0, 0, 0, 0, 0 ,0 , 1, 0, 1 }, zs[n_iters] = { 0 };
    unsigned int y = 6;

    struct parameter vec1[] = { VAR(xs, 'u'), VECTOR_v2(&vec1[0], &n_iters) }, p1 = vec1[1];
    struct parameter vec2[] = { VAR(ys, 'u'), VECTOR_v2(&vec2[0], &n_iters) }, p2 = vec2[1];
    struct parameter vec3[] = { VAR(zs, 'u'), VECTOR_v2(&vec3[0], &out_length) }, p3 = vec3[1];


    FILTER(
        ((struct filter_config) { .condition_fmt = "u<u&u" }),
        VAR(y, 'u'), p1, p2, p3
    );

    BUNDLE_END();

    hotcall_test_teardown();

    // check that orignal list is unmodified.
    for(int i = 0; i < n_iters; ++i) {
        ASSERT_EQ(xs[i], i);
    }

    ASSERT_EQ(out_length, 2);
    ASSERT_EQ(zs[0], 7);
    ASSERT_EQ(zs[1], 9);

}

TEST(filter,7) {
    //Contract:

    /*hotcall_test_setup();

    BUNDLE_BEGIN();

    unsigned int n_iters = 3, out_length;

    int x = 0, y = 1, z = 2;
    void *ptrs[n_iters] = { &x, &y, &z };

    struct parameter vec1[] = { VAR(xs, 'u'), VECTOR_v2(&vec1[0], &n_iters) }, p1 = vec1[1];
    struct parameter vec2[] = { VAR(ys, 'u'), VECTOR_v2(&vec2[0], &n_iters) }, p2 = vec2[1];
    struct parameter vec3[] = { VAR(zs, 'u'), VECTOR_v2(&vec3[0], &out_length) }, p3 = vec3[1];


    FILTER(
        ((struct filter_config) { .condition_fmt = "u<u&u" }),
        VAR(y, 'u'), p1, p2, p3
    );

    BUNDLE_END();

    hotcall_test_teardown();

    // check that orignal list is unmodified.
    for(int i = 0; i < n_iters; ++i) {
        ASSERT_EQ(xs[i], i);
    }

    ASSERT_EQ(out_length, 2);
    ASSERT_EQ(zs[0], 7);
    ASSERT_EQ(zs[1], 9);*/

}
