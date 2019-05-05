#include <cppgenerate/class.h>
#include <cppgenerate/method.h>
#include <cppgenerate/argument.h>
#include <iostream>

int main(int argc, char** argv){
    cppgenerate::Class c( "Example3" );

    c.addMethod( cppgenerate::Method::create()
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

    //For this simple class, we only print the 
    //generated code to stdout
    c.print( std::cout, std::cout );
}
