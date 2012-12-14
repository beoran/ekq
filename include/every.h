#ifndef EVERY_H_INCLUDED
#define EVERY_H_INCLUDED

/**
* Every is an iterator that allows to iterate over every element of
* a collection.
*/

struct Every_;
typedef struct Every_ Every;

struct EveryActs_;
typedef struct EveryActs_ EveryActs;


// types for the funtion pointers that every uses
typedef Every * (EveryNext(Every * every));
typedef void  * (EveryPut(Every * every, void * element));
typedef void  * (EveryDone(Every * every));
typedef Every * (EveryInit(Every * every));

// Struct with all function pointers for Every
struct EveryActs_ {
  EveryDone * done;
  EveryInit * init;
  EveryNext * next;
  EveryPut  * put;
};

/**
* Every is a struct that can be used with collections like Dynar to fetch
* every element of the collection.
*/
struct Every_ {
  void         * now;
  void         * on;
  int            index;
  void         * extra;
  EveryActs    * acts;
};


struct Each_;
typedef struct Each_ Each;
/**
* Each is an alternative iterator interface, that
* requires a function callback in stead of an object. 
*/
struct Each_ {
  void * on;
  void * now;
  void * extra;
  int    index;
};

/** Function pointer for the each iteration method.  */
typedef void * (EachDo(Each * element));

/** An even simpler iterator interface, one for simply iterating and one 
for searching. Iteration is stopped if AllData returns not-NULL, 
and the returned data will be returned by the function that uses AllData 
too. Extra is used for passing in extra data if any. */
typedef void * (AllData)(void * data, void * extra);


/** Generic comparator function. Much like strcmp. */
typedef int (AllCompare)(void * one, void * two);



// get autogenerated function prototypes.
/* This file was generated with:
'cfunctions -c -aoff -n -w every_proto src/every.c' */
#ifndef CFH_EVERY_PROTO
#define CFH_EVERY_PROTO

/* From 'src/every.c': */

/* Macro definitions for C extensions for Cfunctions. */

/* 
   Copyright (C) 1998 Ben K. Bullock.

   This header file is free software; Ben K. Bullock gives unlimited
   permission to copy, modify and distribute it. 

   This header file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/


#ifndef C_EXTENSIONS_H
#define C_EXTENSIONS_H

/* Only modern GNU C's have `__attribute__'.  The keyword `inline'
   seems to have been around since the beginning, but it does not work
   with the `-ansi' option, which defines `__STRICT_ANSI__'.  I expect
   that `__attribute__' does not work with `-ansi' either.  Anyway
   both of these failure cases are being lumped together here for the
   sake of brevity. */

#if defined (__GNUC__) && ( __GNUC__ >= 2 ) && ( __GNUC_MINOR__ > 4 ) && \
   ! defined (__STRICT_ANSI__)

/* Macro definitions for Gnu C extensions to C. */

#define X_NO_RETURN __attribute__((noreturn))
#define X_PRINT_FORMAT(a,b) __attribute__((format(printf,a,b)))
#define X_CONST __attribute__((const))
#define X_INLINE

#else /* Not a modern GNU C */

#define X_NO_RETURN 
#define X_PRINT_FORMAT(a,b) 
#define X_CONST

#endif /* GNU C */

/* The following `#define' is not a mistake.  INLINE is defined to
   nothing for both GNU and non-GNU compilers.  When Cfunctions sees
   `INLINE' it copies the whole of the function into the header file,
   prefixed with `extern inline' and surrounded by an `#ifdef
   X_INLINE' wrapper.  In order to inline the function in GNU C, only
   `X_INLINE' needs to be defined. There is also a normal prototype
   for the case that X_INLINE is not defined.  The reason for copying
   the function with a prefix `extern inline' into the header file is
   explained in the GNU C manual and the Cfunctions manual. */

#define INLINE
#define NO_RETURN void
#define NO_SIDE_FX
#define PRINT_FORMAT(a,b)
#define LOCAL

/* Prototype macro for `traditional C' output. */

#ifndef PROTO
#if defined(__STDC__) && __STDC__ == 1
#define PROTO(a) a
#else
#define PROTO(a) ()
#endif /* __STDC__ */
#endif /* PROTO */

#endif /* ndef C_EXTENSIONS_H */
Every * every_alloc PROTO ((void));

Every * every_done (Every * self );

Every * every_free (Every * self );

Every * every_init (Every * self , EveryActs * acts );

Every * every_new (EveryActs * acts );

void * every_get (Every * self );

void * every_put (Every * self , void * data );

Every * every_next (Every * self , void * data );

Each * each_init (Each * self , void * on , void * data );

Each * each_next (Each * self , void * now );

void * each_now (Each * self );

void * each_on (Each * self );

void * each_extra (Each * self );

int each_index (Each * self );

#endif /* CFH_EVERY_PROTO */

// and some helper macros
#define EACH_NOW(EACH, TYPE) ((TYPE *) each_now(EACH))



#endif
