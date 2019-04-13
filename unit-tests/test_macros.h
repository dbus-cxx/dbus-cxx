#ifndef TEST_MACROS_H
#define TEST_MACROS_H

#define TEST_EQUALS(a,b) (a == b)
#define TEST_STREQUALS(a,b) (std::string(a)==b)

#define TEST_EQUALS_RET_FAIL(a,b) if( TEST_EQUALS(a,b) == false ) return false;
#define TEST_ASSERT_RET_FAIL(a) if(!(a)) return false;

#define STR_EXPAND(tok) #tok
#define STRINGIFY(tok) STR_EXPAND(tok)

#endif
