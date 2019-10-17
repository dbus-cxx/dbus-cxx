#ifndef CPPGENERATE_CODEBLOCK_H
#define CPPGENERATE_CODEBLOCK_H

#include <string>
#include <sstream>

#include <iostream>

namespace cppgenerate{

class CodeBlock{
public:
    CodeBlock();

    CodeBlock( const CodeBlock& other );

    CodeBlock& operator=( const CodeBlock& other );

    CodeBlock& indent();

    CodeBlock& unindent();

    CodeBlock& setIndent( int indent );

    /**
     * Add a line of code to this block
     */
    CodeBlock& addLine( std::string line );

    void print( std::ostream& writer ) const;

    /**
     * Get the internal buffer for this code block
     */
    std::ostringstream& buffer();

    /**
     * Append a block of code to this block
     */
    CodeBlock& operator<<( const CodeBlock& other );

    /**
     * Write this block to the specified stream
     */
    friend std::ostream& operator<<( std::ostream& stream, const CodeBlock& block ){
        stream << block.m_text.str();
        return stream;
    }

    /**
     * same as operator<<( const CodeBlock& other )
     */
    CodeBlock& addCode( const CodeBlock& other );

    static CodeBlock create();

private:
    std::ostringstream m_text;
    int m_indent;
};

}

#endif /* CPPGENERATE_CODEBLOCK_H */
