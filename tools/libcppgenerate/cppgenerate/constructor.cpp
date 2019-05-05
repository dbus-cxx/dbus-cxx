#include "constructor.h"
#include "class.h"

using cppgenerate::Constructor;

Constructor::Constructor(){
}

Constructor::Constructor( const Constructor& other ){
    m_documentation = other.m_documentation;
    m_arguments = other.m_arguments;
    m_code = other.m_code;
    m_accessModifier = other.m_accessModifier;
}

Constructor& Constructor::operator=( const Constructor& other ){
    if( this != &other ){
        m_documentation = other.m_documentation;
        m_arguments.clear();
        m_arguments = other.m_arguments;
        m_code = other.m_code;
        m_accessModifier = other.m_accessModifier;
    }

    return *this;
}

Constructor& Constructor::addArgument( const Argument& arg ){
    m_arguments.push_back( arg );
    
    return *this;
}

Constructor& Constructor::setDocumentation( std::string documentation ){
    m_documentation = documentation;

    return *this;
}

Constructor& Constructor::addCode( const CodeBlock& block ){
    m_code << block;

    return *this;
}

Constructor& Constructor::setCode( const CodeBlock& block ){
    m_code = block;

    return *this;
}

void Constructor::printSignature( const cppgenerate::Class* parent, std::ostream& stream, bool withAccessModifier ) const {
    if( withAccessModifier ){
        switch( m_accessModifier ){
            case AccessModifier::PUBLIC:
                stream << "public:" << std::endl;
                break;
            case AccessModifier::PRIVATE:
                stream << "private:" << std::endl;
                break;
            case AccessModifier::PROTECTED:
                stream << "protected:" << std::endl;
                break;
        }
    }

    if( m_documentation.length() > 1 ){
        stream << "/** " << m_documentation << " */" << std::endl;
    }

    printConstructorSignature( stream, parent->getName(), false );

    stream << ";" << std::endl;
}

void Constructor::printImplementation( const cppgenerate::Class* parent, std::ostream& stream, bool inHeader ) const{
    bool comma = false;
    std::vector<cppgenerate::MemberVariable> initializerVariables;

    printConstructorSignature( stream, parent->getName(), !inHeader );

    for( cppgenerate::MemberVariable memberVar : parent->m_memberVariables ){
        if( memberVar.initializer().size() ){
            initializerVariables.push_back( memberVar );
        }
    }

    if( initializerVariables.size() || parent->m_parents.size() ) stream << " : ";

    for( cppgenerate::Class::Parent clparent : parent->m_parents ){
        if( comma ) stream << "," << std::endl;
        stream << clparent.parentName << "( ";
        if( clparent.initializer.size() ) stream << clparent.initializer;
        stream << " )";
        comma = true;
    }


    for( cppgenerate::MemberVariable memberVar : initializerVariables ){
        if( comma ) stream << ", ";
        stream << memberVar.name() << "( " << memberVar.initializer() << " )" << std::endl;
        comma = true;
    }

    stream << "{" << std::endl;

    stream << m_code << std::endl;
    stream << "}" << std::endl;
}

Constructor Constructor::create(){
    return Constructor();
}

void Constructor::printConstructorSignature( std::ostream& stream, std::string className, bool qualify ) const {
    bool addComma = false;

    if( qualify ){
        stream << className << "::";
    }

    stream << className << "(";

    for( Argument argument : m_arguments ){
        if( addComma ) stream << ", ";
        stream << argument;
        addComma = true;
    }

    stream << " )";
}

Constructor& Constructor::setAccessModifier( cppgenerate::AccessModifier modifier ){
    m_accessModifier = modifier;

    return *this;
}

cppgenerate::AccessModifier Constructor::accessModifier() const {
    return m_accessModifier;
}
