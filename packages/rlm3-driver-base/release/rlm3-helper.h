#pragma once

#include "rlm3-base.h"
#include <Assert.h>


#ifdef __cplusplus
extern "C" {
#endif


#define THIRTY_SECOND_ARG(X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25, X26, X27, X28, X29, X30, X31, X32, ...) X32
#define COUNT_ARGS(...) THIRTY_SECOND_ARG(ignore, ##__VA_ARGS__, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define CONCAT(a, b) a ## b
#define CONCAT2(a, b) CONCAT(a, b)

#define FLAG(FLAG_NAME, VALUE) (FLAG_NAME), ((VALUE) << (FLAG_NAME ## _Pos))

#define SET_REGISTER_FLAGS(REG, ...) CONCAT2(SET_REGISTER_FLAGS_, COUNT_ARGS(__VA_ARGS__))(REG, 0, 0, __VA_ARGS__)
#define SET_REGISTER_FLAGS_0(REG, CLEAR_MASK, SET_MASK) MODIFY_REG(REG, CLEAR_MASK, SET_MASK)
#define SET_REGISTER_FLAGS_2(REG, CLEAR_MASK, SET_MASK, F1, V1) SET_REGISTER_FLAGS_0(REG, ((CLEAR_MASK) | (F1)), ((SET_MASK) | (V1)))
#define SET_REGISTER_FLAGS_4(REG, CLEAR_MASK, SET_MASK, F1, V1, ...) SET_REGISTER_FLAGS_2(REG, ((CLEAR_MASK) | (F1)), ((SET_MASK) | (V1)), __VA_ARGS__)
#define SET_REGISTER_FLAGS_6(REG, CLEAR_MASK, SET_MASK, F1, V1, ...) SET_REGISTER_FLAGS_4(REG, ((CLEAR_MASK) | (F1)), ((SET_MASK) | (V1)), __VA_ARGS__)
#define SET_REGISTER_FLAGS_8(REG, CLEAR_MASK, SET_MASK, F1, V1, ...) SET_REGISTER_FLAGS_6(REG, ((CLEAR_MASK) | (F1)), ((SET_MASK) | (V1)), __VA_ARGS__)
#define SET_REGISTER_FLAGS_10(REG, CLEAR_MASK, SET_MASK, F1, V1, ...) SET_REGISTER_FLAGS_8(REG, ((CLEAR_MASK) | (F1)), ((SET_MASK) | (V1)), __VA_ARGS__)
#define SET_REGISTER_FLAGS_12(REG, CLEAR_MASK, SET_MASK, F1, V1, ...) SET_REGISTER_FLAGS_10(REG, ((CLEAR_MASK) | (F1)), ((SET_MASK) | (V1)), __VA_ARGS__)
#define SET_REGISTER_FLAGS_14(REG, CLEAR_MASK, SET_MASK, F1, V1, ...) SET_REGISTER_FLAGS_12(REG, ((CLEAR_MASK) | (F1)), ((SET_MASK) | (V1)), __VA_ARGS__)
#define SET_REGISTER_FLAGS_16(REG, CLEAR_MASK, SET_MASK, F1, V1, ...) SET_REGISTER_FLAGS_14(REG, ((CLEAR_MASK) | (F1)), ((SET_MASK) | (V1)), __VA_ARGS__)


#ifdef __cplusplus
}
#endif