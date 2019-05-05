#include "variable.h"

using cppgenerate::Variable;

Variable::Variable(){
}

Variable::Variable( const Variable& other ){
    m_initializer = other.m_initializer;
    m_type = other.m_type;
    m_name = other.m_name;
}

Variable::~Variable(){
}

Variable& Variable::operator=( const Variable& other ){
    if( this != &other ){
        m_initializer = other.m_initializer;
        m_type = other.m_type;
        m_name = other.m_name;
    }
    return *this;
}

Variable& Variable::setInitializer( std::string initializer ){
    m_initializer = initializer;
    return *this;
}

Variable& Variable::setType( std::string type ){
    m_type = type;
    return *this;
}

Variable& Variable::setName( std::string name ){
    m_name = name;
    return *this;
}

Variable Variable::create(){
    return Variable();
}

std::string Variable::initializer(){
    return m_initializer;
}

std::string Variable::name(){
    return m_name;
}
