#ifndef MACROFOREACH_H
#define MACROFOREACH_H

// Make a FOREACH macro
#define FE_1(WHAT, X) WHAT(X)
#define FE_2(WHAT, X, ...) WHAT(X)FE_1(WHAT, __VA_ARGS__)
#define FE_3(WHAT, X, ...) WHAT(X)FE_2(WHAT, __VA_ARGS__)
#define FE_4(WHAT, X, ...) WHAT(X)FE_3(WHAT, __VA_ARGS__)
#define FE_5(WHAT, X, ...) WHAT(X)FE_4(WHAT, __VA_ARGS__)
//#define FE_6(WHAT, X, ...) WHAT(X)FE_5(WHAT, __VA_ARGS__)
//#define FE_7(WHAT, X, ...) WHAT(X)FE_6(WHAT, __VA_ARGS__)
//#define FE_8(WHAT, X, ...) WHAT(X)FE_7(WHAT, __VA_ARGS__)
//#define FE_9(WHAT, X, ...) WHAT(X)FE_8(WHAT, __VA_ARGS__)
//... repeat as needed

#define GET_MACRO(_1,_2,_3,_4,_5,NAME,...) NAME
#define FOR_EACH(action,...) \
  GET_MACRO(__VA_ARGS__,FE_5,FE_4,FE_3,FE_2,FE_1)(action,__VA_ARGS__)

/**
// Example
// Some actions
#define QUALIFIER(X) X::
#define OPEN_NS(X)   namespace X {
#define CLOSE_NS(X)  }
// Helper function
#define QUALIFIED(NAME,...) FOR_EACH(QUALIFIER,__VA_ARGS__)NAME

// Emit some code
QUALIFIED(MyFoo,Outer,Next,Inner)  foo();

FOR_EACH(OPEN_NS,Outer,Next,Inner)
  class Foo;
FOR_EACH(CLOSE_NS,Outer,Next,Inner)

*/

#endif // MACROFOREACH_H
