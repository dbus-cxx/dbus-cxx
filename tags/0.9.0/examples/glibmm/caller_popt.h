/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
 *   rvinyard@cs.nmsu.edu                                                  *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 *                                                                         *
 *   The dbus-cxx library is free software; you can redistribute it and/or *
 *   modify it under the terms of the GNU General Public License           *
 *   version 3 as published by the Free Software Foundation.               *
 *                                                                         *
 *   The dbus-cxx library is distributed in the hope that it will be       *
 *   useful, but WITHOUT ANY WARRANTY; without even the implied warranty   *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this software. If not see <http://www.gnu.org/licenses/>.  *
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

#define CALLER_PARSE_ARGS(argc, argv)                                                                                                            \
                                                                                                                                                 \
  poptContext opt_context;                                                                                                                       \
  char* op = NULL;                                                                                                                               \
  double param1=NAN, param2=NAN;                                                                                                                 \
                                                                                                                                                 \
  struct poptOption option_table[] = {                                                                                                           \
    { "param1", '1', POPT_ARG_DOUBLE,   &param1, '1', "First parameter [default=3.3]" },                                                         \
    { "param2", '2', POPT_ARG_DOUBLE,   &param2, '2', "Second parameter [default=4.4]" },                                                        \
    { "op",    'o', POPT_ARG_STRING, &op,        'o', "The operation to execute (add, sub, mul, div) [default=add] (case sensitive)" },          \
    POPT_AUTOHELP                                                                                                                                \
    { NULL, 0, 0, NULL, 0 }                                                                                                                      \
  };                                                                                                                                             \
                                                                                                                                                 \
  opt_context = poptGetContext(NULL, argc, argv, option_table, 0 );                                                                              \
                                                                                                                                                 \
  while ( (poptGetNextOpt(opt_context)) >= 0) { }                                                                                                \
                                                                                                                                                 \
  if ( op == NULL ) op = strdup("add");                                                                                                          \
                                                                                                                                                 \
  if ( isnan(param1) ) param1 = 3.3;                                                                                                             \
                                                                                                                                                 \
  if ( isnan(param2) ) param2 = 4.4;                                                                                                             \
                                                                                                                                                 \
  if ( not (strcmp(op,"add")==0 or strcmp(op,"sub")==0 or strcmp(op,"mul")==0 or strcmp(op,"div")==0) ) {                                        \
    fprintf( stderr, "ERROR: Operation '%s' is invalid.\n\nOperation is case sensitive and must be one of:\n\tadd\n\tsub\n\tmul\n\tdiv\n", op ); \
    return 1;                                                                                                                                    \
  }                                                                                                                                              \
                                                                                                                                                 \
  char opsym;                                                                                                                                    \
                                                                                                                                                 \
  switch(op[0])                                                                                                                                  \
  {                                                                                                                                              \
    case 'a': opsym = '+'; break;                                                                                                                \
    case 's': opsym = '-'; break;                                                                                                                \
    case 'm': opsym = '*'; break;                                                                                                                \
    case 'd': opsym = '/'; break;                                                                                                                \
  }

