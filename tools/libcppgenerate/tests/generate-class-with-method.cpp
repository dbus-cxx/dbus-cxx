#include <cppgenerate/class.h>
#include <iostream>
#include <fstream>

int main(){
    cppgenerate::Class c( "ClassWithMethod" );

    c.addMethod( cppgenerate::Method::create()
        .setName( "exampleMethod" )
        .setAccessModifier( cppgenerate::AccessModifier::PUBLIC ) );
    
    std::ofstream header;
    header.open( "ClassWithMethod.h" );

    std::ofstream cpp;
    cpp.open( "ClassWithMethod.cpp" );

    c.print( header, cpp );

    header.close();
    cpp.close();
}
