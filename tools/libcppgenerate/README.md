# libcppgenerate

A library for generating C++ code from code.

Based off of libkode, but designed to have both a DSL API and not tied to Qt.

## Building

```
mkdir build
cd build
cmake ..
make 
make install
```

## Example


libcppgenerate uses a fluent API in order to allow for easy creation.
Alternatively, you can have the function calls do only one thing.

### Fluent example
```
/* Create a class called MathOperations */
cppgenerate::Class c( "MathOperations" );

/* Fluent interface: chain all of the method calls together */
c.setNamespace( "Math" )
   .addSystemInclude( "string" )
   .addMethod( cppgenerate::Method::create()
      .setName( "add" )
      /* The documentation is in javadoc/doxygen compatible format */
      .setDocumentation( "Adds two integers" )
      .addArgument( cppgenerate::Argument()
               .setType( "int" )
               .setName( "arg1" ) )
      .addArgument( cppgenerate::Argument()
               .setType( "int" )
               .setName( "arg2" ) )
      .setReturnType( "int" )
      .setCode( cppgenerate::CodeBlock::create().addLine( "return arg1 + arg2;" ) )
  )
  .addMethod( cppgenerate::Method::create()
      .setName( "square" )
      .addArgument( cppgenerate::Argument()
               .setType( "int" )
               .setName( "arg1" ) )
      .setReturnType( "int" )
      .setCode( cppgenerate::CodeBlock::create().addLine( "return arg1 * arg1;" ) )
  );
  

  /* Set the header that we want to write out */
  std::ofstream header;
  header.open( "MathOperations.h" );

  /* Set the implementation that we want to print out */
  std::ofstream cpp;
  cpp.open( "MathOperations.cpp" );

  /* Print the header and implementation */
  c.print( header, cpp );
```

### Imperative Example
```
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
```

# License

Apache 2.0
