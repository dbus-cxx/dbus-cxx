#include <ostream>

#include "membervariable.h"

using cppgenerate::MemberVariable;

MemberVariable::MemberVariable() :
    m_access( AccessModifier::PUBLIC ),
    m_generateGetter( false ),
    m_generateSetter( false ),
    m_isQProperty( false ){
}

MemberVariable::~MemberVariable(){
}

MemberVariable::MemberVariable( const MemberVariable& other ) :
    Variable( other ) {
    m_access = other.m_access;
    m_generateGetter = other.m_generateGetter;
    m_generateSetter = other.m_generateSetter;
    m_isQProperty = other.m_isQProperty;
}

MemberVariable& MemberVariable::operator=( const MemberVariable& other ){
    if( this != &other ){
        Variable::operator=( other );
        m_access = other.m_access;
        m_generateGetter = other.m_generateGetter;
        m_generateSetter = other.m_generateSetter;
        m_isQProperty = other.m_isQProperty;
        m_changedSignalName = other.m_changedSignalName;
    }

    return *this;
}

MemberVariable& MemberVariable::setAccessModifier( AccessModifier modifier ){
    m_access = modifier;
    return *this;
}

MemberVariable& MemberVariable::generateSetter( bool generateSetter ){
    m_generateSetter = generateSetter;
    return *this;
}

MemberVariable& MemberVariable::generateGetter( bool generateGetter ){
    m_generateGetter = generateGetter;
    return *this;
}

MemberVariable& MemberVariable::isQProperty( bool isQproperty ){
    m_isQProperty = isQproperty;
    return *this;
}

MemberVariable& MemberVariable::setChangedSignalName( std::string signalName ){
    m_changedSignalName = signalName;
    return *this;
}

MemberVariable MemberVariable::create(){
    return MemberVariable();
}

void MemberVariable::print( std::ostream& stream, int indent, bool withAccessModifier ) const {
    cppgenerate::CodeBlock block;

    if( m_name.size() == 0 || m_type.size() == 0 ){
        //Can't generate code for this memvar
        return;
    }

    if( withAccessModifier ){
        switch( m_access ){
            case AccessModifier::PUBLIC:
                block.addLine( "public:");
                break;
            case AccessModifier::PRIVATE:
                block.addLine( "private:");
                break;
            case AccessModifier::PROTECTED:
                block.addLine( "protected:");
                break;
        }
    }

    block.indent()
        .addLine( m_type + " " + m_name + ";" )
        .unindent();

    if( m_generateSetter ){
        block.addLine( "public:" )
            .addLine( "void set" + m_name + "( " + m_type + " arg ){" )
            .indent()
            .addLine( m_name + " = arg;" )
            .unindent()
            .addLine( "}" );
    }

    if( m_generateGetter ){
        block.addLine( "public:" )
            .addLine( m_type + " get" + m_name + "(){" )
            .indent()
            .addLine( "return " + m_name + ";" )
            .unindent()
            .addLine( "}" );
    }

    block.print( stream );
}

cppgenerate::AccessModifier MemberVariable::accessModifier() const{
    return m_access;
}
