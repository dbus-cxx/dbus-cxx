#include <cppgenerate/enum.h>
#include "test_macros.h"

int main(){
    cppgenerate::Enum e;

    e.addEnumValue( "foobar", 5 );

    std::map<std::string, int> values = e.getEnumNameToValues();

    return !TEST_EQUALS( values["foobar"], 5 );
}
