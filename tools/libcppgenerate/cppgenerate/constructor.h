#ifndef CPPGENERATE_CONSTRUCTOR_H
#define CPPGENERATE_CONSTRUCTOR_H

#include <list>

#include "argument.h"
#include "codeblock.h"
#include "accessmodifier.h"

namespace cppgenerate{

class Class;

/**
 * A Constructor is very similar to a Method, except that you 
 * can't set a name or return type.
 */
class Constructor {
  public:
    Constructor();

    Constructor( const Constructor& other );

    Constructor& operator=( const Constructor& other );

    Constructor& setAccessModifier( AccessModifier modifier );

    /**
     * Add an argument to the constructor
     */
    Constructor& addArgument( const Argument& arg );

    /**
     * Set the documentation for the constructor
     */
    Constructor& setDocumentation( std::string documentation );

    /**
     * Add a block of code.  Appends to the currently existing code.
     */
    Constructor& addCode( const CodeBlock& block );

    /**
     * Set the code for this method
     */
    Constructor& setCode( const CodeBlock& block );

    /**
     * Print the constructor signature to the specified stream
     */
    void printSignature( const Class* parent, std::ostream& stream, bool withAccessModifier ) const;

    /**
     * Print the constructor implementation to the specified stream
     */
    void printImplementation( const Class* parent, std::ostream& stream, bool inHeader ) const;

    AccessModifier accessModifier() const;

    static Constructor create();

  private:
    void printConstructorSignature( std::ostream& stream, std::string className, bool quailify ) const;

  private:
    std::string m_documentation;
    std::list<Argument> m_arguments;
    CodeBlock m_code;
    AccessModifier m_accessModifier;
};

} /* namespace cppgenerate */

#endif /* CPGENERATE_CONSTRUCTOR_H */
