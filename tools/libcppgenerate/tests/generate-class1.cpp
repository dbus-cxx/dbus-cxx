#include <cppgenerate/class.h>
#include <iostream>
#include <fstream>

int main( int argc, char** argv ){
    cppgenerate::Class c( "FooClass" );

    c.setNamespace( "Bar" )
     .addSystemInclude( "string" )
     .addMethod( cppgenerate::Method::create()
        .setName( "add" )
        .setDocumentation( "Adds two integers" )
        .addArgument( cppgenerate::Argument()
                 .setType( "int" )
                 .setName( "arg1" ) )
        .addArgument( cppgenerate::Argument()
                 .setType( "int" )
                 .setName( "arg2" ) )
        .setReturnType( "int" )
        .setCode( cppgenerate::CodeBlock::create().addLine( "return arg1 + arg2;" ) )
    );

    std::ofstream header;
    header.open( "FooClass.h" );

    std::ofstream cpp;
    cpp.open( "FooClass.cpp" );

    c.print( header, cpp );

    header.close();
    cpp.close();
}
