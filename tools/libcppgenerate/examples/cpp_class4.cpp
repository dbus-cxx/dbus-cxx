#include <cppgenerate/class.h>
#include <cppgenerate/method.h>
#include <cppgenerate/argument.h>
#include <iostream>

int main(int argc, char** argv){
    cppgenerate::Class c( "Example4" );

    c.addConstructor( cppgenerate::Constructor::create()
         .setDocumentation( "Default Constructor" ) )
      .addConstructor( cppgenerate::Constructor::create()
         .addArgument( cppgenerate::Argument::create()
           .setType( "int" )
           .setName( "foo" ) )
         .addCode( cppgenerate::CodeBlock::create().addLine( "m_foo = foo;" ) ) )
      .addMemberVariable( cppgenerate::MemberVariable::create()
          .setType( "int" )
          .setName( "m_foo" )
          .setInitializer( "5" ) );

    //For this simple class, we only print the 
    //generated code to stdout
    c.print( std::cout, std::cout );
}
