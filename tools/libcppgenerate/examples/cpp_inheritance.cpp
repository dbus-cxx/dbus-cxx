#include <cppgenerate/class.h>
#include <cppgenerate/method.h>
#include <cppgenerate/argument.h>
#include <iostream>

int main(int argc, char** argv){
    cppgenerate::Class c( "Inheritance" );

    //This example shows how to add inheritance to a class.
    c.addParentClass( "FooClass", cppgenerate::AccessModifier::PUBLIC )
     .addLocalInclude( "Foo" )
     .addConstructor( cppgenerate::Constructor::create() );

    //For this simple class, we only print the 
    //generated code to stdout
    c.print( std::cout, std::cout );
}
