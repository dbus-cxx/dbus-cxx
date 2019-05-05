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

    friend std::ostream& operator<<( std::ostream& stream, const Argument& argument ){
        if( argument.m_isConst ){
            stream << "const ";
        }

        stream << argument.m_type;
        switch( argument.m_argPassType ){
        case ArgumentPassType::REFERENCE:
            stream << "&";
            break;
        case ArgumentPassType::POINTER:
            stream << "*";
            break;
        default:
            break;
        }

        stream << " " << argument.m_name;

        if( argument.m_defaultValue.size() ){
            stream << " = " << argument.m_defaultValue;
        }
        return stream;
    }

    std::string type() const;

    std::string name() const;

    static Argument create();

  private:
    std::string m_type;
    std::string m_name;
    std::string m_defaultValue;
    bool m_isConst;
    ArgumentPassType m_argPassType;
};

}

#endif 
