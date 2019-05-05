#include <cppgenerate/enum.h>
#include "test_macros.h"

int main(){
    cppgenerate::Enum e;

    e.addEnumValue( "what", 6 );
    e.addEnumValue( "stoop" );

    std::map<std::string, int> values = e.getEnumNameToValues();

    return !TEST_EQUALS( values["stoop"], 7 );
}
