#ifndef CPPGENERATE_VARIABLE_H
#define CPPGENERATE_VARIABLE_H

#include <string>

namespace cppgenerate{

/**
 * Represents a variable in the generated code.
 */
class Variable{
  public:
    Variable();

    Variable( const Variable& other );

    virtual ~Variable();

    Variable& operator=( const Variable& other );

    Variable& setInitializer( std::string initializer );

    Variable& setType( std::string type );

    Variable& setName( std::string name );

    std::string initializer();

    std::string name();

    static Variable create();

  protected:
    std::string m_type;
    std::string m_name;
    std::string m_initializer;
};

}

#endif /* CPPGENERATE_VARIABLE_H */
