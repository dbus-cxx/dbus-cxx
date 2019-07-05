#ifndef CPPGENERATE_ARGUMENT_H
#define CPPGENERATE_ARGUMENT_H

#include <string>
#include <ostream>

namespace cppgenerate{

/**
 * Determines how arguments are passed to a function
 */
enum class ArgumentPassType{
    DEFAULT, /* Argument is passed in directly */
    REFERENCE, /* Argument is passed in as & */
    POINTER /* Argument is a * */
};

/**
 * This class represents an argument to a function
 */
class Argument{
  public:
    Argument();

    Argument( const Argument& other );

    Argument& operator=( const Argument& other );

    Argument& setType( std::string type );

    Argument& setName( std::string name );

    Argument& setDefaultValue( std::string defaultValue );

    Argument& setIsConst( bool isConst );

    Argument& setPassType( ArgumentPassType passType );

    std::string type() const;

    std::string name() const;

    void printWithDefaultValue( std::ostream& stream ) const;
    void printWithoutDefaultValue( std::ostream& stream ) const;

    static Argument create();

  private:
    void print( std::ostream& stream, bool withDefaultValue ) const;

  private:
    std::string m_type;
    std::string m_name;
    std::string m_defaultValue;
    bool m_isConst;
    ArgumentPassType m_argPassType;
};

}

#endif 
