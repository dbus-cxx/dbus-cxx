#ifndef CPPGENERATE_METHOD_H
#define CPPGENERATE_METHOD_H

#include <string>
#include <vector>

#include "codeblock.h"
#include "argument.h"
#include "accessmodifier.h"

namespace cppgenerate{

class Class;

/**
 * Represents a single method, generally on a class
 */
class Method{
  public:

    Method();

    Method( const Method& other );

    ~Method();

    Method& operator=( const Method& other );

    /**
     * Checks to see if this method is valid.  A method is valid if it
     * has a name.
     */
    bool isValid() const;

    /**
     * Set the name of this method
     */
    Method& setName( std::string name );

    Method& setReturnType( std::string returnType );

    /**
     * Add an argument to the method
     */
    Method& addArgument( const Argument& arg );

    /**
     * Set the documentation for the method
     */
    Method& setDocumentation( std::string documentation );

    /**
     * Add a block of code.  Appends to the currently existing code.
     */
    Method& addCode( const CodeBlock& block );

    /**
     * Set the code for this method
     */
    Method& setCode( const CodeBlock& block );

    Method& setAccessModifier( AccessModifier modifier );

    Method& setStatic( bool isStatic );

    Method& setVirtual( bool isVirtual );

    /**
     * Set if this method is pure virtual.  Also sets virtual, no need to also call setVirtual
     */
    Method& setPureVirtual( bool isPureVirutal );

    /**
     * Print the method signature to the specified stream
     *
     * @param stream Where to write the signature to
     * @param indent The level of indent for this signature.
     * @param withAccessModifier True if the access level should be printed, false otherwise
     */
    void printSignature( std::ostream& stream, int indent, bool withAccessModifier ) const;

    /**
     * Print the method implementation to the specified stream
     */
    void printImplementation( const Class* parent, std::ostream& stream, bool inHeader ) const;

    std::vector<Argument> arguments() const;

    std::string name() const;

    std::string returnType() const;

    AccessModifier accessModifier() const;

    static Method create();

  private:
    void printMethodSignature( std::ostream& stream, std::string className ) const;

  private:
    std::string m_name;
    std::string m_returnType;
    std::string m_documentation;
    std::vector<Argument> m_arguments;
    CodeBlock m_code;
    AccessModifier m_access;
    bool m_isStatic;
    bool m_isVirtual;
    bool m_isPureVirtual;
};

}

#endif /* CPPGENERATE_METHOD_H */
