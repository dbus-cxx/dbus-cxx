#include <cppgenerate/enum.h>
#include "test_macros.h"

int main(){
    cppgenerate::Enum e;

    e.setName( "}FOO{" );

    return !TEST_STREQUALS( "_FOO_", e.getName() );
}
