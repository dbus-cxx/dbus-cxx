#include <iostream>

#include "printer.h"

using cppgenerate::Printer;

Printer::Printer() :
    m_generateAsSingleFile( false )
{}

Printer::Printer( const Printer& other ) :
    m_generateAsSingleFile( other.m_generateAsSingleFile ),
    m_classes( other.m_classes )
{}

Printer& Printer::addClass( const Class& cls ){
    m_classes.push_back( cls );

    return *this;
}

Printer& Printer::generateAsSingleFile( bool singleFile ){
    m_generateAsSingleFile = singleFile;

    return *this;
}

Printer& Printer::setOutputDirectory( std::string outputDirectory ){
    m_outputDirectory = outputDirectory;

    return *this;
}

Printer& Printer::print(){
    //std::ostream single;

    if( m_generateAsSingleFile ){
        //single = std::ostream( "jksldaf" );
    }

    for( const Class& cls : m_classes ){
        //std::ostream output( std::cout );

        //if( m_generateAsSingleFile ) output = single;
        cls.print( std::cout, std::cout );
    }
}

void Printer::printClass( const Class& cls, std::ostream ostream ){

}
