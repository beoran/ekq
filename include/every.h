#ifndef EVERY_H

struct Every_;
typedef struct Every_ Every;

struct EveryActs_;
typedef struct EveryActions_ EveryActs;


// types for the funtion pointers that every uses
typedef (Every *)(EveryNext(Every * every));
typedef (void  *)(EveryPut(Every * every, void * element));
typedef (void  *)(EveryDone(Every * every));
typedef (Every *)(EveryInit(Every * every));

// Struct with all function pointers for Every
struct EveryActs_ {
  EveryDone * done;
  EveryInit * init;
  EveryNext * next;
  EveryPut  * put;
}

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


// get autogenerated function prototypes.
#include "every_proto.h"



#endif