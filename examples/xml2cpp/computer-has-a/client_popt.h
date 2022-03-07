/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
 *   rvinyard@cs.nmsu.edu                                                  *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#include <string.h>
#include <math.h>
#include <popt.h>

// Parse command line options
// This is boilerplate for all the caller-* examples
// Useful symbols declared:
//   op: C string containing the op to perform
//   opsym: C char containing the mathmatical symbol of the op
//   param1: double containing parameter 1
//   param2: double containing parameter 2

#define CLIENT_PARSE_ARGS(argc, argv)                                                                                                                             \
                                                                                                                                                                  \
  poptContext opt_context;                                                                                                                                        \
  char* op = NULL;                                                                                                                                                \
  double param1=NAN, param2=NAN;                                                                                                                                  \
                                                                                                                                                                  \
  const char* op_help_str = "The operation to execute (add, sub, mul, div, pi, print-pi, factorial, fibonacci, thue-morse) [default=fibonacci] (case sensitive)"; \
                                                                                                                                                                  \
  const char* valid_ops[] = { "add", "sub", "mul", "div", "pi", "print-pi", "factorial", "fibonacci", "thue-morse" };                                             \
                                                                                                                                                                  \
  struct poptOption option_table[] = {                                                                                                                            \
    { "param1", '1', POPT_ARG_DOUBLE,   &param1, '1', "First parameter [default=3.3]" },                                                                          \
    { "param2", '2', POPT_ARG_DOUBLE,   &param2, '2', "Second parameter [default=4.4]" },                                                                         \
    { "op",    'o', POPT_ARG_STRING, &op,        'o', op_help_str },                                                                                              \
    POPT_AUTOHELP                                                                                                                                                 \
    { NULL, 0, 0, NULL, 0 }                                                                                                                                       \
  };                                                                                                                                                              \
                                                                                                                                                                  \
  opt_context = poptGetContext(NULL, argc, argv, option_table, 0 );                                                                                               \
                                                                                                                                                                  \
  while ( (poptGetNextOpt(opt_context)) >= 0) { }                                                                                                                 \
                                                                                                                                                                  \
  if ( op == NULL ) op = strdup("fibonacci");                                                                                                                     \
                                                                                                                                                                  \
  if ( std::isnan(param1) ) param1 = 3.3;                                                                                                                              \
                                                                                                                                                                  \
  if ( std::isnan(param2) ) param2 = 4.4;                                                                                                                              \
                                                                                                                                                                  \
  bool valid_op = false;                                                                                                                                          \
                                                                                                                                                                  \
  for ( int i = 0; i < 9; i++ )                                                                                                                                   \
  {                                                                                                                                                               \
    if ( strcmp(op,valid_ops[i]) == 0 )                                                                                                                           \
    {                                                                                                                                                             \
      valid_op = true;                                                                                                                                            \
      break;                                                                                                                                                      \
    }                                                                                                                                                             \
  }                                                                                                                                                               \
                                                                                                                                                                  \
  if ( not valid_op ) {                                                                                                                                           \
    fprintf( stderr, "ERROR: Operation '%s' is invalid.\n\nOperation is case sensitive and must be one of:\n", op );                                              \
    for ( int i=0; i < 9; i++ ) fprintf( stderr, "\t%s\n", valid_ops[i] );                                                                                        \
    return 1;                                                                                                                                                     \
  }
