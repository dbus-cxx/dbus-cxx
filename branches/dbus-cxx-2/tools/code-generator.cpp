/***************************************************************************
 *   Copyright (C) 2018 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 *                                                                         *
 *   The dbus-cxx library is free software; you can redistribute it and/or *
 *   modify it under the terms of the GNU General Public License           *
 *   version 3 as published by the Free Software Foundation.               *
 *                                                                         *
 *   The dbus-cxx library is distributed in the hope that it will be       *
 *   useful, but WITHOUT ANY WARRANTY; without even the implied warranty   *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this software. If not see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#include <iostream>
#include <fstream>
#include <dbus-cxx.h>

#include "code-generator.h"

using DBus::CodeGenerator;

CodeGenerator::CodeGenerator(){
    m_parser = XML_ParserCreate(NULL);
    XML_SetElementHandler( m_parser, CodeGenerator::start_element_handler, CodeGenerator::end_element_handler );
    XML_SetUserData( m_parser, this );
}

CodeGenerator::~CodeGenerator(){
    XML_ParserFree( m_parser );
}

void CodeGenerator::setXML( const std::string& xmlDocument ){
    m_xmlData = xmlDocument;
}

bool CodeGenerator::parse(){
    if( XML_Parse( m_parser, m_xmlData.c_str(), m_xmlData.size(), 1 ) == XML_STATUS_ERROR ){
        std::cerr << "XML Error: " << XML_ErrorString(XML_GetErrorCode(m_parser)) << std::endl;
        std::cerr << "Line:" << XML_GetCurrentLineNumber(m_parser) << std::endl;
        return false;
    }

    return true;
}

void CodeGenerator::start_element( std::string tagName, std::map<std::string,std::string> tagAttrs ){
    m_tagStack.push( tagName );

    if( tagName.compare( "node" ) == 0 ){
        cppgenerate::Class newclass;
        std::string dest;
        std::string path;

        if( tagAttrs.find( "gen-namespace" ) != tagAttrs.end() ){
            newclass.setNamespace( tagAttrs.find( "gen-namespace" )->second );
        }

        if( tagAttrs.find( "dest" ) != tagAttrs.end() ){
            dest = "\"" + tagAttrs[ "dest" ] + "\"";
        }else{
            std::cerr << "WARNING: Did not find 'dest' in XML for node" << std::endl;
        }

        if( tagAttrs.find( "path" ) != tagAttrs.end() ){
            path = "\"" + tagAttrs[ "path" ] + "\"";
        }else{
            std::cerr << "WARNING: Did not find 'path' in xml for node" << std::endl;
        }

        if( tagAttrs.find( "cppname" ) != tagAttrs.end() ){
            newclass.setName( tagAttrs[ "cppname" ] );
        }else{
            newclass.setName( "NONAME" );
        }


        newclass.addSystemInclude( "dbus-cxx.h" )
          .addSystemInclude( "stdint.h" )
          .addSystemInclude( "string" )
          .addSystemInclude( "memory" )
          .addParentClass( "DBus::ObjectProxy", cppgenerate::AccessModifier::PUBLIC, "conn, dest, path" );

        newclass.addMethod( cppgenerate::Method::create()
            .setAccessModifier( cppgenerate::AccessModifier::PUBLIC )
            .setName( "create" )
            .setStatic( true )
            .setReturnType( "std::shared_ptr<" + newclass.getName() + ">" )
            .addCode( cppgenerate::CodeBlock::create()
                .addLine( "return std::shared_ptr<" + newclass.getName() + ">( new " + newclass.getName() + "( conn, dest, path );" ) )
            .addArgument( cppgenerate::Argument::create()
              .setType( "DBus::Connection::Pointer" )
              .setName( "conn" ) )
            .addArgument( cppgenerate::Argument::create()
              .setType( "std::string" )
              .setName( "dest" )
              .setDefaultValue( dest ) )
            .addArgument( cppgenerate::Argument::create()
              .setType( "std::string" )
              .setName( "path" )
              .setDefaultValue( path ) ) );

        m_currentProxyConstructor = cppgenerate::Constructor::create()
            .setAccessModifier( cppgenerate::AccessModifier::PROTECTED )
            .addArgument( cppgenerate::Argument::create()
              .setType( "DBus::Connection::Pointer" )
              .setName( "conn" ) )
            .addArgument( cppgenerate::Argument::create()
              .setType( "std::string" )
              .setName( "dest" )
              .setDefaultValue( dest ) )
            .addArgument( cppgenerate::Argument::create()
              .setType( "std::string" )
              .setName( "path" )
              .setDefaultValue( path ) );
              
        m_proxyClasses.push_back( newclass );
    }else if( tagName.compare( "interface" ) == 0 ){
        if( tagAttrs.find( "name" ) == tagAttrs.end() ){
            std::cerr << "WARNING: No name for interface found" << std::endl;
            return;
        }
        m_currentInterface = tagAttrs[ "name" ];
    }else if( tagName.compare( "method" ) == 0 ){
        if( tagAttrs.find( "name" ) == tagAttrs.end() ){
            std::cerr << "WARNING: No name for method found" << std::endl;
            return;
        }
        m_currentProxyMethod = cppgenerate::Method()
            .setName( tagAttrs[ "name" ] )
            .setAccessModifier( cppgenerate::AccessModifier::PUBLIC );

    }else if( tagName.compare( "arg" ) == 0 ){
        cppgenerate::Argument arg;

        if( tagAttrs.find( "direction" ) == tagAttrs.end() ){
            //XML_GetCurrentLineNumber
            std::cerr << "WARNING: No direction for arg found(assuming in)." << std::endl;
        }

        DBus::Signature signature( tagAttrs[ "type" ] );

        DBus::Signature::iterator it = signature.begin();
        //typestr = type_string_from_code( it.type() );

        arg.setName( tagAttrs[ "name" ] )
           .setType( type_string_from_code( it.type() ) );

        if( tagAttrs[ "direction" ] == "out" ){
            m_currentProxyMethod.setReturnType( type_string_from_code( it.type() ) );
        }else{
            m_currentProxyMethod.addArgument( arg );
        }
    }
}

void CodeGenerator::end_element( std::string tagName ){
    m_tagStack.pop();

    if( tagName == "method" ){
        /* Add a method to the class, along with its implementation
         * to actually call over the bus
         */
        cppgenerate::MemberVariable memberVar;
        std::vector<cppgenerate::Argument> args = m_currentProxyMethod.arguments();
        std::string dbusSig;
        /* method proxy type = template args */
        std::string methodProxyType;
        /* methodArguments = arguments to proxy method */
        std::string methodArguments;
        bool argumentComma = false;

        methodProxyType += "<" + m_currentProxyMethod.returnType();
        for( cppgenerate::Argument arg : args ){
            methodProxyType += ",";
            methodProxyType += arg.type();

            if( argumentComma ) methodArguments += ",";
            methodArguments += arg.name();
            argumentComma = true;
        }
        methodProxyType += ">";

        memberVar.setAccessModifier( cppgenerate::AccessModifier::PROTECTED )
                 .setName( "m_method_" + m_currentProxyMethod.name() )
                 .setType( "DBus::MethodProxy " + methodProxyType );

        m_currentProxyMethod.addCode( cppgenerate::CodeBlock::create()
            .addLine( "return (*" + memberVar.name() + ")(" +  methodArguments + ");" ) );

        m_proxyClasses.data()[ m_proxyClasses.size() - 1 ]
            .addMethod( m_currentProxyMethod )
            .addMemberVariable( memberVar );

        m_currentProxyConstructor.addCode( cppgenerate::CodeBlock::create()
            .addLine( "m_method_" + m_currentProxyMethod.name() + 
                      " = this->create_method" + methodProxyType + 
                      "(\"" + m_currentInterface + "\",\"" + m_currentProxyMethod.name() + "\");" ) );
    }else if( tagName == "node" ){
        m_proxyClasses.data()[ m_proxyClasses.size() - 1 ]
            .addConstructor( m_currentProxyConstructor );
    }
}

void CodeGenerator::start_element_handler(void* user_data, const XML_Char* name, const XML_Char** attrs ){
    CodeGenerator* generator = static_cast<CodeGenerator*>( user_data );
    std::string tagName = std::string( name );
    std::map<std::string,std::string> tagAttrs;
    int i = 0;

    while( attrs[ i ] != NULL ){
        std::string attrKey( attrs[ i ] );
        std::string attrValue( attrs[ i + 1 ] );

        tagAttrs.insert( std::make_pair( attrKey, attrValue ) );

        i += 2;
    }

    generator->start_element( tagName, tagAttrs );
}

void CodeGenerator::end_element_handler( void* userData, const XML_Char* name ){
    CodeGenerator* generator = static_cast<CodeGenerator*>( userData );
    std::string tagName = std::string( name );

    generator->end_element( tagName );
}

void CodeGenerator::generateProxyClasses(){
    for( cppgenerate::Class c : m_proxyClasses ){
        std::string headerFilename = c.getName() + std::string( "_proxy.h" );
        std::string implFilename = c.getName() + std::string( "_proxy.cpp" );
        std::ofstream header;
        std::ofstream impl;

        header.open( headerFilename );
        impl.open( implFilename );

        c.print( header, impl );
    }
}
