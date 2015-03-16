#ifndef _TEST_UTIL_H_
#define _TEST_UTIL_H_

#include <stdio.h>

#define test_suite(NAME, BLOCK)                                   \
  int (**test_cases)(void);                                       \
  int main(int argc, char **argv) {                               \
    int tests  = 0;                                               \
    int successful = 0;                                           \
    printf("Running suite " #NAME "\n");                          \
    BLOCK;                                                        \
    printf("Finished suite " #NAME "\n");                         \
    printf("%d of %d tests was successful\n", successful, tests); \
    if (tests > successful) {                                     \
      return 1;                                                   \
    } else {                                                      \
      return 0;                                                   \
    }                                                             \
  }

#define test_case(NAME, DEF)                           \
  do {                                                 \
    int ok = 0;                                        \
    tests++;                                           \
    do {                                               \
      DEF;                                             \
      successful++;                                    \
      ok = 1;                                          \
    } while (0);                                       \
    if (ok) {                                          \
      printf("OK   %s\n", #NAME);                      \
    } else {                                           \
      printf("FAIL %s\n", #NAME);                      \
    }                                                  \
  } while (0)

#define assert_eq(EXPR1, EXPR2)                                         \
  if ((EXPR1) != (EXPR2)) {                                             \
    printf("assertion failed: '%s == %s'\n"                             \
           "  expected: %p but got %p at %s:%d\n",                      \
           #EXPR1, #EXPR2, (EXPR1), (EXPR2), __FILE__, __LINE__);       \
    break;                                                              \
  }

#define assert(EXPR)                                   \
  if (!(EXPR)) {                                       \
    printf("assertion failed: '%s'\n"                  \
           "  at %s:%d\n",                             \
         #EXPR, __FILE__, __LINE__);                   \
    break;                                             \
  }

#endif
