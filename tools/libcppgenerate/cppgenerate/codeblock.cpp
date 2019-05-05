#include "codeblock.h"

using cppgenerate::CodeBlock;

CodeBlock::CodeBlock() : 
    m_indent( 0 )
{}

CodeBlock::CodeBlock( const CodeBlock& other ) :
    m_indent( other.m_indent )
{
    m_text << other.m_text.str();
}

CodeBlock& CodeBlock::operator=( const CodeBlock& other ){
    if( this != &other ){
        m_indent = other.m_indent;
        m_text.str( "" );
        m_text << other.m_text.str();
    }

    return *this;
}

CodeBlock& CodeBlock::indent(){
    m_indent += 4;

    return *this;
}

CodeBlock& CodeBlock::unindent(){
    m_indent -= 4;
    if( m_indent < 0 ){
        m_indent = 0;
    }

    return *this;
}

CodeBlock& CodeBlock::setIndent( int indent ){
    m_indent = indent;
    if( m_indent < 0 ){
        m_indent = 0;
    }

    return *this;
}

CodeBlock& CodeBlock::addLine( std::string line ){
    m_text << std::string( m_indent, ' ' ) << line << std::endl;

    return *this;
}

void CodeBlock::print( std::ostream& writer ) const{
    writer << m_text.str();
}

std::ostringstream& CodeBlock::buffer(){
    return m_text;
}

CodeBlock& CodeBlock::operator<<( const CodeBlock& other ){
    m_text << other.m_text.str();

    return *this;
}

CodeBlock& CodeBlock::addCode( const CodeBlock& other ){
    m_text << other;

    return *this;
}

CodeBlock CodeBlock::create(){
    return CodeBlock();
}
