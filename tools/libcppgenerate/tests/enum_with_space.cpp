#include <cppgenerate/enum.h>
#include "test_macros.h"

int main(){
    cppgenerate::Enum e;

    e.setName( "NEW NAME" );

    return !TEST_STREQUALS( "NEW_NAME", e.getName() );
}
