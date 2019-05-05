#include <cppgenerate/class.h>
#include <iostream>
#include <fstream>

int main(){
    cppgenerate::Class c( "NamespaceClass" );

    c.setNamespace( "Bar" );
    
    std::ofstream header;
    header.open( "NamespaceClass.h" );

    std::ofstream cpp;
    cpp.open( "NamespaceClass.cpp" );

    c.print( header, cpp );

    header.close();
    cpp.close();
}
