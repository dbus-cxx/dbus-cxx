#include "argument.h"

using cppgenerate::Argument;

Argument::Argument() :
    m_isConst( false ),
    m_argPassType( ArgumentPassType::DEFAULT )
{}

Argument::Argument( const Argument& other ){
    m_type = other.m_type;
    m_name = other.m_name;
    m_defaultValue = other.m_defaultValue;
    m_isConst = other.m_isConst;
    m_argPassType = other.m_argPassType;
}

Argument& Argument::operator=( const Argument& other ){
    if( this != &other ){
        m_type = other.m_type;
        m_name = other.m_name;
        m_defaultValue = other.m_defaultValue;
        m_isConst = other.m_isConst;
        m_argPassType = other.m_argPassType;
    }

    return *this;
}

Argument& Argument::setType( std::string type ){
    m_type = type;

    return *this;
}

Argument& Argument::setName( std::string name ){
    m_name = name;

    return *this;
}

Argument& Argument::setDefaultValue( std::string value ){
    m_defaultValue = value;

    return *this;
}

Argument& Argument::setIsConst( bool isConst ){
    m_isConst = isConst;

    return *this;
}

Argument& Argument::setPassType( ArgumentPassType passType ){
    m_argPassType = passType;

    return *this;
}

Argument Argument::create(){
    return Argument();
}

std::string Argument::name() const {
    return m_name;
}

std::string Argument::type() const {
    return m_type;
}
