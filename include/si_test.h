#ifndef _TEST_H_
#define _TEST_H_

/* Simple unit testing.*/
struct Test_ ; 
typedef struct Test_ Test;

struct Test_ {
  int count;
  int ok;
  int failed;
};

/* 
*  The header file carries it's own implementation around,
*  which is fine for simple test programs. 
*/   
#ifndef TESTING_NO_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

// some cute ANSI colors, can be disabled by defining TESTING_NO_ANSI
#ifndef TESTING_NO_ANSI
#define TEST_ANSI_R(S) ("\033[1;31m" S "\033[0m")
#define TEST_ANSI_G(S) ("\033[1;32m" S "\033[0m")
#define TEST_ANSI_Y(S) ("\033[1;33m" S "\033[0m")
#else
#define TEST_ANSI_R(S) (S)
#define TEST_ANSI_G(S) (S)
#define TEST_ANSI_Y(S) (S)
#endif

#define TEST_UNUSED(x) ((void)(x))

/* Epsilon for double and float comparisons*/
#ifndef TEST_DOUBLE_EPSILON
#define TEST_DOUBLE_EPSILON 0.0000000001
#endif 

#ifndef TEST_FLOAT_EPSILON
#define TEST_FLOAT_EPSILON 0.0000001
#endif 


/** Initializes the test. */
static Test * test_init(Test * test) {
  test->failed = test->ok = test->count = 0;
  return test; 
}

/** Adds one OK test.*/
static Test * test_ok(Test * test) {
  test->count ++;
  test->ok    ++;
  return test;  
}


/** Fails one test with variable arguments. */
static Test * test_fail_va(Test * test, const char * fmt, va_list ap) {
  test->count ++;
  test->failed++;
  fprintf(stderr, TEST_ANSI_R("Test %d failed: "), test->count);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  return test;
} 


/** Fails one test. */
static Test * test_fail(Test * test, const char * fmt, ...) {
  va_list  ap;  
  va_start(ap, fmt);
  test_fail_va(test, fmt, ap);
  va_end(ap); 
  return test;
}


/** 
Reports the results of the tests. Retuns nonzero if there were errors, zero if no errors, so it can be called in main as return test_report(&test);
*/
static int test_report(Test * test) {
  if (test->ok == test->count) {
    fprintf(stderr, TEST_ANSI_G("SUCCESS: %d/%d \n"), test->ok, test->count);
    return 0;
  } else {
    fprintf(stderr, TEST_ANSI_R("FAILED: %d/%d \n"), test->ok, test->count);
    return 1;
  }
}

/** Tests if an assertion is true with variable arguments. */
static Test * test_assert_va(Test * test, int assert, const char * fmt, 
                             va_list ap) {
  if(assert) { // if the assertion is true 
    test_ok(test);
  } else {
    test_fail_va(test, fmt, ap);
  }
  return test;
}


/** Tests if a certain asertion is true, failing if it false. */
static Test * test_assert(Test * test, int assert, const char * fmt, ...) {
  va_list  ap;  
  va_start(ap, fmt);
  test_assert_va(test, assert, fmt, ap);
  va_end(ap); 
  return test;  
}


/** Tests if a pointer is null, faling the test if it's not so. */
static Test * test_null(Test * test, void * ptr, const char * explain) {
  return test_assert(test, ptr == NULL, 
              "Pointer should be null: %p; %s",  ptr, explain); 
}

/** Tests if a pointer is not null, faling the test if it is NULL. */
static Test * test_notnull(Test * test, void * ptr, const char * explain) {
  return test_assert(test, ptr != NULL, 
  "Pointer should be not null: %p; %s",  ptr, explain);
}



/** Tests if two strings are equal according to strcmp. */
static Test * test_streq(Test * test, char * s1, char * s2, const char * explain) {
  return test_assert(test, strcmp(s1, s2) == 0, 
  "Strings should be equal: >%s< >%s<; %s ", s1, s2, explain);
}

/** Tests if two strings are equal according to strncmp. */
static Test * test_streqn(Test * test, char * s1, size_t n, char * s2, 
                          const char * explain) {
  return test_assert(test, strncmp(s1, s2, n) == 0, 
  "Strings should be equal: >%s< >%s<; %s ", s1, s2, explain);
}

/** Tests if two memory areas are equal according to memcmp. */
static Test * test_memeq(Test * test, void * m1, size_t n, void * m2,  
                  const char * explain) {
  if(!m2) return test_fail(test, 
"Memory should be equal but pointer was null: %p %p; %s", m1, m2, explain);
  return test_assert(test, memcmp(m1, m2, n) == 0, 
  "Memory should be equal: %p %p; %s ", m1, m2, explain);
}

/** Tests if two pointers are equal */
static Test * test_ptreq(Test * test, void *p1, void * p2, const char * explain)
{
  return test_assert(test, p1 == p2, 
  "Pointers should be equal: %p %p; %s ", p1, p2, explain);
}

/** Tests if two pointers are unequal */
static Test * test_ptrneq(Test * test, void *p1, void * p2, 
              const char * explain) {
  return test_assert(test, p1 != p2, 
  "Pointers should not be equall: %p %p; %s ", p1, p2, explain);
}

/** Tests if two integers are equal */
static Test * test_inteq(Test * test, int i1, int i2, const char * explain) {
  return test_assert(test, i1 == i2, 
  "Integers should be equal: %d %d; %s ", i1, i2, explain);
}

/** Tests if two integers are not equal */
static Test * test_intneq(Test * test, int i1, int i2, const char * explain) {
  return test_assert(test, i1 != i2, 
  "Integers should not be equal: %d %d; %s ", i1, i2, explain);
}

/** Tests if two long integers are equal */
static Test * test_longeq(Test * test, long i1, long i2, const char * explain) {
  return test_assert(test, i1 == i2, 
  "Integers should be equal: %ld %ld; %s ", i1, i2, explain);
}

/** Tests if two long integers are not equal */
static Test * test_longneq(Test * test, long i1, long i2, const char * explain) {
  return test_assert(test, i1 != i2, 
  "Integers should not be equal: %ld %ld; %s ", i1, i2, explain);
}

/** Tests if two doubles are equal within TEST_DOUBLE_EPSILON */
static Test * test_doubleeq(Test * test, double d1, double d2, const char * explain) {
  double delta = fabs(d2 - d1);
  return test_assert(test, delta < TEST_DOUBLE_EPSILON, 
  "Doubles should not be equal: %lf %lf; %s ", d1, d2, explain);
}

/** Tests if two doubles are not more different than TEST_DOUBLE_EPSILON */
static Test * test_doubleneq(Test * test, double d1, double d2, const char * explain) {
  double delta = fabs(d2 - d1);
  return test_assert(test, delta >= TEST_DOUBLE_EPSILON, 
  "Doubles should not be equal: %lf %lf; %s ", d1, d2, explain);
}

/** Tests if two floats are equal within TEST_FLOAT_EPSILON */
static Test * test_floateq(Test * test, float d1, float d2, const char * explain) {
  float delta = fabsf(d2 - d1);
  return test_assert(test, delta < TEST_FLOAT_EPSILON, 
  "Doubles should not be equal: %lf %lf; %s ", d1, d2, explain);
}

/** Tests if two floats are not more different than TEST_FLOAT_EPSILON */
static Test * test_floatneq(Test * test, float d1, float d2, const char * explain) {
  float delta = fabsf(d2 - d1);
  return test_assert(test, delta >= TEST_FLOAT_EPSILON, 
  "Doubles should not be equal: %lf %lf; %s ", d1, d2, explain);
}


/** Tests if an integer has a true value */
static Test * test_true(Test * test, int boole, const char * explain) {
  return test_assert(test, boole, 
  "Boolean should be TRUE: %d; %s ", boole, explain);
}

/** Tests if an integer has a false value */
static Test * test_false(Test * test, int boole, const char * explain) {
  return test_assert(test, (!boole), 
  "Boolean should be FALSE: %d; %s ", boole, explain);
}

/** This only exists to supress spurious compiler warnings. */
static Test * warning_supressor(Test * test) {
  if(!test) return NULL;
  test_streq(test, "", "", "");
  test_streqn(test, "", 0, "", "");
  test_memeq(test, "", 0, "", "");
  test_inteq(test, 0, 0, "");
  test_intneq(test, 0, 0, "");
  test_true(test, 0, "");
  test_false(test, 0, "");
  test_null(test, 0, "");
  test_notnull(test, 0, "");
  test_ptreq(test, NULL, NULL, "");
  test_ptrneq(test, NULL, NULL, "");
  TEST_UNUSED(warning_supressor);
  return test_fail(test, "");
}



/* Macros to help with calling tested functions.
* Important notice: these macros assume that a Test * _t exists.
*/  
#define PP_TOSTR_AID(VAL)             #VAL
#define PP_TOSTR(VAL)                 PP_TOSTR_AID(VAL)

#define PP_PASTE(FIRST, SECOND)       FIRST##SECOND
#define PP_JOINSTR(FIRST, SECOND)    (FIRST SECOND)
#define PP_JOINSTR3(A1, A2, A3)      (A1 A2 A3)

#define PP_LINE_STR()                 PP_TOSTR(__LINE__)
#define PP_FILE_STR()                 PP_TOSTR(__FILE__)
#define PP_LONGLINE_STR()             PP_JOINSTR(" line nr:", PP_LINE_STR())  

#define TEST_LINE(CALL)               \
        PP_JOINSTR(PP_TOSTR(CALL), " " PP_FILE_STR() " line nr:" PP_LINE_STR())
#define TEST_CAID(CALL, TEXT)         CALL,TEXT
#define TEST_CALL(CALL)               TEST_CAID(CALL, TEST_LINE(CALL))
#define TEST_TRUE(CALL)               test_true(_t,   (int)TEST_CALL(CALL))
#define TEST_FALSE(CALL)              test_false(_t,  (int)TEST_CALL(CALL))
#define TEST_ASSERT(CALL)             test_assert(_t,      TEST_CALL(CALL))
#define TEST_NULL(CALL)               test_null(_t,        TEST_CALL(CALL))
#define TEST_NOTNULL(CALL)            test_notnull(_t,     TEST_CALL(CALL))
#define TEST_STREQ(STR, CALL)         \
        test_streq(_t,  (char *)STR, (char *)TEST_CALL(CALL))
#define TEST_STREQN(STR, SIZE, CALL)  \
        test_streqn(_t, (char *)STR, SIZE, (char *)TEST_CALL(CALL))
#define TEST_MEMEQ(MEM, SIZE, CALL)   test_memeq(_t,  MEM, SIZE,TEST_CALL(CALL))
#define TEST_PTREQ(PTR, CALL)         test_ptreq(_t,  PTR, TEST_CALL(CALL))
#define TEST_PTRNEQ(PTR, CALL)        test_ptrneq(_t,  PTR, TEST_CALL(CALL))
#define TEST_INTEQ(INT, CALL)         test_inteq(_t,  INT, TEST_CALL(CALL))
#define TEST_INTNEQ(INT, CALL)        test_intneq(_t, INT, TEST_CALL(CALL))
#define TEST_LONGEQ(INT, CALL)        test_longeq(_t,  INT, TEST_CALL(CALL))
#define TEST_LONGNEQ(INT, CALL)       test_longneq(_t, INT, TEST_CALL(CALL))
#define TEST_DOUBLEEQ(INT, CALL)      test_doubleeq(_t,  INT, TEST_CALL(CALL))
#define TEST_DOUBLENEQ(INT, CALL)     test_doubleneq(_t, INT, TEST_CALL(CALL))
#define TEST_FLOATEQ(INT, CALL)       test_floateq(_t,  INT, TEST_CALL(CALL))
#define TEST_FLOATNEQ(INT, CALL)      test_floatneq(_t, INT, TEST_CALL(CALL))


#define TEST_ZERO(CALL)               TEST_INTEQ(0, CALL)

/* Other utility macros. They are syntactic, so you may dislike them.
   But, they are very handy for unit testing C. :) 
*/
#define TEST_FUNC(NAME)       Test * test_##NAME(Test * _t) 
#define TEST_DONE()           return _t
#define TEST_RUN(NAME)        test_##NAME(_t)
#define TEST_INIT()           Test _st ;  Test * _t ; _t = &_st ; test_init(_t)
#define TEST_REPORT()         return test_report(_t);




#endif




#endif
