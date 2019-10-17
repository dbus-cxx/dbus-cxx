#include <cppgenerate/enum.h>
#include "test_macros.h"

int main(){
    cppgenerate::Enum e;

    e.setName( "NEWNAME" );

    return !TEST_STREQUALS( "NEWNAME", e.getName() );
}
