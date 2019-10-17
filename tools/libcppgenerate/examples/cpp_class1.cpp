#include <cppgenerate/class.h>
#include <iostream>

int main(int argc, char** argv){
    cppgenerate::Class c( "FooClass" );

    //Create a class Bar::FooClass.  It has no
    //methods, members or variables
    c.setNamespace( "Bar" )
     .addSystemInclude( "string" );

    //For this simple class, we only print the
    //generated code to stdout
    c.print( std::cout, std::cout );
}
