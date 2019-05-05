#include <cppgenerate/class.h>
#include <iostream>
#include <fstream>

int main(){
    /* Create a class called MathOperations */
    cppgenerate::Class c( "MathOperations" );

    /* imperative: same API as fluent, but can be useful in some situations */
    c.setNamespace( "Math" );
    c.addSystemInclude( "string" );

    cppgenerate::Method addMethod;
    addMethod.setName( "add" );
    addMethod.setDocumentation( "Adds two integers" );
    addMethod.setAccessModifier( cppgenerate::AccessModifier::PUBLIC );

    cppgenerate::Argument addArg1;
    addArg1.setType( "int" );
    addArg1.setName( "arg1" );

    addMethod.addArgument( addArg1 );

    cppgenerate::Argument addArg2;
    addArg2.setType( "int" );
    addArg2.setName( "arg2" );

    addMethod.addArgument( addArg2 );
    addMethod.setReturnType( "int" );

    cppgenerate::CodeBlock block;
    block.addLine( "return arg1 + arg2;" );

    addMethod.setCode( block );
    c.addMethod( addMethod );

    cppgenerate::Method squareMethod;
    squareMethod.setName( "square" );
    squareMethod.setAccessModifier( cppgenerate::AccessModifier::PUBLIC );

    cppgenerate::Argument squareArgument;
    squareArgument.setType( "int" );
    squareArgument.setName( "arg1" );

    squareMethod.addArgument( squareArgument );
    squareMethod.setReturnType( "int" );

    cppgenerate::CodeBlock squareBlock;
    squareBlock.addLine( "return arg1 * arg1;" );

    squareMethod.setCode( squareBlock );
    c.addMethod( squareMethod );

    std::ofstream header;
    header.open( "MathOperations.h" );

    std::ofstream cpp;
    cpp.open( "MathOperations.cpp" );

    c.print( header, cpp );
}
