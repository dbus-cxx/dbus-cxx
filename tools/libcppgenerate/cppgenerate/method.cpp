#include <utility>
#include <ostream>

#include <iostream>

#include "method.h"
#include "class.h"
#include "cppgenerateutils.h"

using cppgenerate::Method;

Method::Method() : 
    m_returnType( "void" ),
    m_access( cppgenerate::AccessModifier::PRIVATE ),
    m_isVirtual( false ),
    m_isPureVirtual( false ),
    m_isStatic( false ){
    m_code.setIndent( 4 );
}

Method::Method( const Method& other ){
    m_name = other.m_name;
    m_returnType = other.m_returnType;
    m_documentation = other.m_documentation;
    m_arguments = other.m_arguments;
    m_code = other.m_code;
    m_access = other.m_access;
    m_isStatic = other.m_isStatic;
    m_isVirtual = other.m_isVirtual;
    m_isPureVirtual = other.m_isPureVirtual;
}

Method::~Method(){
}

Method& Method::operator=( const Method& other ){
    if( this != &other ){
        m_name = other.m_name;
        m_returnType = other.m_returnType;
        m_documentation = other.m_documentation;
        m_arguments = other.m_arguments;
        m_code = other.m_code;
        m_access = other.m_access;
        m_isStatic = other.m_isStatic;
        m_isVirtual = other.m_isVirtual;
        m_isPureVirtual = other.m_isPureVirtual;
    }

    return *this;
}

bool Method::isValid() const {
    return m_name.size() > 1 && m_returnType.size() > 1;
}

Method& Method::setName( std::string name ){
    m_name = name;

    return *this;
}

Method& Method::setReturnType( std::string returnType ){
    m_returnType = returnType;

    return *this;
}

Method& Method::addArgument( const Argument& arg ){
    m_arguments.push_back( arg );
    
    return *this;
}

Method& Method::setDocumentation( std::string documentation ){
    m_documentation = documentation;

    return *this;
}

Method& Method::addCode( const CodeBlock& block ){
    m_code << block;

    return *this;
}

Method& Method::setCode( const CodeBlock& block ){
    m_code = block;

    return *this;
}

void Method::printSignature( std::ostream& stream, int indent, bool withAccessModifier ) const {
    if( !isValid() ) return;

    if( withAccessModifier ){
        switch( m_access ){
        case cppgenerate::AccessModifier::PRIVATE: stream << "private:" << std::endl; break;
        case cppgenerate::AccessModifier::PUBLIC: stream << "public:" << std::endl; break;
        case cppgenerate::AccessModifier::PROTECTED: stream << "protected:" << std::endl; break;
        }
    }

    if( m_documentation.length() > 1 ){
        cppgenerate::insertSpaces( stream, indent );
        stream << "/** " << m_documentation << " */" << std::endl;
    }

    cppgenerate::insertSpaces( stream, indent );
    if( m_isStatic ) stream << "static ";
    if( m_isVirtual ) stream << "virtual ";
    printMethodSignature( stream, "" );

    if( m_isPureVirtual ) stream << " = 0";
    stream << ";" << std::endl;
}

void Method::printImplementation( const cppgenerate::Class* parent, std::ostream& stream, bool inHeader ) const{
    std::string parentName;

    if( !isValid() ) return;

    if( !inHeader && parent != nullptr ) parentName = parent->getName();
    if( inHeader && m_isVirtual ) stream << "virtual ";

    printMethodSignature( stream, parentName );

    if( !m_isPureVirtual ){
        stream << "{" << std::endl;

        stream << m_code << std::endl;
        stream << "}" << std::endl;
    }

    if( inHeader && m_isPureVirtual ){
        stream << " = 0;" << std::endl;
    }
}

Method Method::create(){
    return Method();
}

void Method::printMethodSignature( std::ostream& stream, std::string className ) const {
    bool addComma = false;

    stream << m_returnType << " ";

    if( className.size() > 0 ){
        stream << className << "::";
    }

    stream << m_name << "(";

    for( Argument argument : m_arguments ){
        if( addComma ) stream << ", ";
        stream << argument;
        addComma = true;
    }

    stream << " )";
}

std::vector<cppgenerate::Argument> Method::arguments() const {
    return m_arguments;
}

std::string Method::name() const{
    return m_name;
}

std::string Method::returnType() const {
    return m_returnType;
}

Method& Method::setAccessModifier( AccessModifier modifier ){
    m_access = modifier;

    return *this;
}

cppgenerate::AccessModifier Method::accessModifier() const{
    return m_access;
}

Method& Method::setStatic( bool isStatic ){
    m_isStatic = isStatic;

    return *this;
}

Method& Method::setVirtual( bool isVirtual ){
    m_isVirtual = isVirtual;

    return *this;
}

Method& Method::setPureVirtual( bool isPureVirtual ){
    if( isPureVirtual ){
        m_isVirtual = true;
    }
    m_isPureVirtual = isPureVirtual;

    return *this;
}
