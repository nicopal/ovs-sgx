#include "ecalls.h"
#include <string.h>

bool
ecall_always_true() {
  return true;
}

bool
ecall_always_false() {
  return false;
}

void
ecall_foo() {}

void
ecall_bar() {}

void
ecall_plus_one(int *x) {
    ++*x;
}

int
ecall_plus_one_ret(int x) {
    return ++x;
}

bool
ecall_greater_than_two(int *x) {
    return *x > 2 ? true : false;
}


bool
ecall_greater_than_y(int *x, int y) {
    return *x > y ? true : false;
}

void
ecall_plus_plus(int *x, int *y) {
    *x = *x + 1;
    *y = *y + 2;
}

int
ecall_plus(int x, int y) {
    return x + y;
}

void
ecall_plus_y(int *x, int y) {
    *x = *x + y;
}

void
ecall_plus_y_v2(int x, int *y) {
    *y = x + *y;
}

bool
ecall_revert(bool x) {
    return x == true ? false : true;
}

void
ecall_zero(int *x) {
    *x = 0;
}

int buffer[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

int
ecall_count() {
    return sizeof(buffer) /sizeof(int);
}

int
ecall_read_buffer(int *out, int size) {
    unsigned buffer_size = ecall_count();
    int n = (buffer_size > size ? size : buffer_size);
    memcpy(out, buffer, n * sizeof(int));
    return n;
}
