/***************************************************************************
 *   Copyright (C) 2018-2020 by Robert Middleton                           *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#include <iostream>
#include <fstream>
#include <dbus-cxx.h>
#include <string>

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
    std::string parentElement = m_tagStack.empty() ? "" : m_tagStack.top();
    m_tagStack.push( tagName );

    if( tagName.compare( "node" ) == 0 ){
        handle_node_tag( tagAttrs );
    }else if( tagName.compare( "interface" ) == 0 ){
        std::string interfaceName;
        if( tagAttrs.find( "name" ) == tagAttrs.end() ){
            std::cerr << "WARNING: No name for interface found" << std::endl;
            return;
        }
        interfaceName = tagAttrs[ "name" ];
        std::string cppName = interfaceName;
        std::replace( cppName.begin(), cppName.end(), '.', '_' );
        m_currentInterface = InterfaceInfo( interfaceName );

        std::map<std::string,std::string>::iterator it = tagAttrs.find( "cppname" );
        if( it != tagAttrs.end() ){
            cppName = it->second;
        }
        m_currentInterface.setCppname( cppName );

        m_ignoreInterface = false;
        it = tagAttrs.find( "ignored" );
        if( it != tagAttrs.end() ){
            if( it->second == "true" ){
                m_ignoreInterface = true;
            }
        }
    }else if( tagName.compare( "method" ) == 0 ){
        handle_method_tag( tagAttrs );
    }else if( tagName.compare( "signal" ) == 0 ){
        handle_signal_tag( tagAttrs );
    }else if( tagName.compare( "arg" ) == 0 ){
        handle_arg_tag( parentElement, tagAttrs );
    }else if( tagName.compare( "property" ) == 0 ){
        handle_property_tag( tagAttrs );
    }
}

void CodeGenerator::end_element( std::string tagName ){
    m_tagStack.pop();

    if( tagName == "method" ){
        m_currentInterface.addMethodInfo( m_currentMethodInfo );
    }else if( tagName == "interface" ){
        if( m_ignoreInterface ) return;
        m_rootNode.addInterfaceInfo( m_currentInterface );
    }else if( tagName == "signal" ){
        m_currentInterface.addSignalInfo( m_currentSignal );
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

void CodeGenerator::generateProxyClasses( bool outputToFile, const std::string& output_directory ){
    std::vector<cppgenerate::Class> proxyClasses;
    cppgenerate::Class mainProxyClass;
    cppgenerate::Constructor mainProxyConstructor;
    std::string mainClassName = m_rootNode.cppname() + "Proxy";

    mainProxyClass.setName( mainClassName )
          .setNamespace( m_rootNode.genNamespace() )
          .addSystemInclude( "dbus-cxx.h" )
          .addSystemInclude( "stdint.h" )
          .addSystemInclude( "string" )
          .addSystemInclude( "memory" )
          .addParentClass( "DBus::ObjectProxy", cppgenerate::AccessModifier::PUBLIC, "conn, dest, path" );

    mainProxyClass.addMethod( cppgenerate::Method::create()
        .setAccessModifier( cppgenerate::AccessModifier::PUBLIC )
        .setName( "create" )
        .setStatic( true )
        .setReturnType( "std::shared_ptr<" + mainClassName + ">" )
        .addCode( cppgenerate::CodeBlock::create()
            .addLine( "std::shared_ptr<" + mainClassName + "> created = std::shared_ptr<" + mainClassName + ">( new " + mainClassName + "( conn, dest, path ) );" )
                  .addLine( "conn->register_object_proxy( created, signalCallingThread );")
                  .addLine( "return created;" ) )
        .addArgument( cppgenerate::Argument::create()
          .setType( "std::shared_ptr<DBus::Connection>" )
          .setName( "conn" ) )
        .addArgument( cppgenerate::Argument::create()
          .setType( "std::string" )
          .setName( "dest" )
          .setDefaultValue( m_rootNode.destination() ) )
        .addArgument( cppgenerate::Argument::create()
          .setType( "std::string" )
          .setName( "path" )
          .setDefaultValue( m_rootNode.name() ) )
        .addArgument( cppgenerate::Argument::create()
          .setType( "DBus::ThreadForCalling" )
          .setName( "signalCallingThread" )
          .setDefaultValue( "DBus::ThreadForCalling::DispatcherThread" ) ) );

    mainProxyConstructor.addArgument( cppgenerate::Argument::create()
          .setType( "std::shared_ptr<DBus::Connection>" )
          .setName( "conn" ) )
        .addArgument( cppgenerate::Argument::create()
          .setType( "std::string" )
          .setName( "dest" )
          .setDefaultValue( m_rootNode.destination() ) )
        .addArgument( cppgenerate::Argument::create()
          .setType( "std::string" )
          .setName( "path" )
          .setDefaultValue( m_rootNode.name() ) );

    for( InterfaceInfo i : m_rootNode.interfaces() ){
        if( i.name() == DBUS_CXX_PEER_INTERFACE ||
                i.name() == DBUSCXX_INTERFACE_INTROSPECTABLE ||
                i.name() == DBUS_CXX_PROPERTIES_INTERFACE ){
            // This is a standard interface, ignore code generation.
            continue;
        }

        cppgenerate::Class newClass;
        std::string proxyClassName = i.cppname() + "Proxy";
        std::string proxyClassNameWithNamespace = proxyClassName;
        if( m_rootNode.genNamespace().length() > 0 ){
            proxyClassNameWithNamespace = m_rootNode.genNamespace() + "::" + proxyClassName;
        }
        newClass.setName( proxyClassName )
                .addSystemInclude( "dbus-cxx.h" )
                .addSystemInclude( "stdint.h" )
                .addSystemInclude( "string" )
                .addSystemInclude( "memory" )
                .setNamespace( m_rootNode.genNamespace() )
                .addParentClass( "DBus::InterfaceProxy", cppgenerate::AccessModifier::PUBLIC, "name" )
                .addMethod( cppgenerate::Method::create()
                            .setName( "create" )
                            .setAccessModifier( cppgenerate::AccessModifier::PUBLIC )
                            .setStatic( true )
                            .setReturnType( "std::shared_ptr<" + proxyClassNameWithNamespace + ">" )
                            .addCode( cppgenerate::CodeBlock::create()
                                      .addLine( "return std::shared_ptr<" + proxyClassNameWithNamespace + ">( new " + proxyClassNameWithNamespace + "( name ));") )
                            .addArgument( cppgenerate::Argument::create()
                                          .setName( "name" )
                                          .setType( "std::string" )
                                          .setDefaultValue( "\"" + i.name() + "\"" ))
                );

        cppgenerate::Constructor proxyConstructor =
                cppgenerate::Constructor::create()
                 .setAccessModifier( cppgenerate::AccessModifier::PROTECTED )
                 .addArgument( cppgenerate::Argument::create()
                   .setType( "std::string" )
                   .setName( "name" ) );

        generateProxyMethods( &newClass, &proxyConstructor, i.methods() );
        generateProxySignals( &newClass, &proxyConstructor, i.signals() );
        generateProxyProperties( &newClass, &proxyConstructor, i.properties() );

        newClass.addConstructor( proxyConstructor );

        // Add the new class to our master proxy class
        mainProxyClass.addMemberVariable( cppgenerate::MemberVariable::create()
                                          .setAccessModifier( cppgenerate::AccessModifier::PROTECTED )
                                          .setName( "m_" + proxyClassName )
                                          .setType( "std::shared_ptr<" + proxyClassNameWithNamespace + ">" ) )
                .addLocalInclude( newClass.getName() + ".h" )
                .addMethod( cppgenerate::Method::create()
                            .setName( "get" + i.cppname() + "Interface" )
                            .setAccessModifier( cppgenerate::AccessModifier::PUBLIC )
                            .setReturnType( "std::shared_ptr<" + proxyClassNameWithNamespace + ">" )
                            .addCode( cppgenerate::CodeBlock::create()
                                      .addLine( "return m_" + proxyClassName + ";" ))
                            );
        mainProxyConstructor.addCode( cppgenerate::CodeBlock::create()
                                      .addLine( "m_" + proxyClassName + " = " + proxyClassNameWithNamespace + "::create( \"" + i.name() + "\" );")
                                      .addLine( "this->add_interface( m_" + proxyClassName + " );" ) );

        proxyClasses.push_back( newClass );
    }

    mainProxyClass.addConstructor( mainProxyConstructor );
    proxyClasses.push_back( mainProxyClass );

    for( cppgenerate::Class c : proxyClasses ){
        if( outputToFile ){
            std::string headerFilename = output_directory + c.getName() + std::string( ".h" );
            std::string implFilename = output_directory + c.getName() + std::string( ".cpp" );
            std::ofstream header;
            std::ofstream impl;

            header.open( headerFilename );
            impl.open( implFilename );

            c.print( header, impl );

            std::cerr << "Generated proxy file: " << headerFilename << "\n";
            std::cerr << "Generated proxy file: " << implFilename << "\n";
        }else{
            c.printAsHeaderOnly( std::cout );
        }
    }

}

void CodeGenerator::generateProxyMethods( cppgenerate::Class *cls, cppgenerate::Constructor* constructor, std::vector<MethodInfo> methods ){
    for( MethodInfo i : methods ){
        cppgenerate::Method newMethod;
        cppgenerate::MemberVariable memberVar;
        std::vector<cppgenerate::Argument> args = i.arguments();
        std::string dbusSig;
        /* method proxy type = template args */
        std::string methodProxyType;
        /* methodArguments = arguments to proxy method */
        std::string methodArguments;
        bool argumentComma = false;
        std::string block;

        newMethod.setName( i.name() )
                .setReturnType( i.returnType() );

        methodProxyType += "<" + i.returnType();
        methodProxyType += "(";
        for( cppgenerate::Argument arg : args ){
            if( argumentComma ){
                methodArguments += ",";
                methodProxyType += ",";
            }

            methodProxyType += arg.type();
            methodArguments += arg.name();
            argumentComma = true;

            newMethod.addArgument( arg );
        }
        methodProxyType += ")";
        methodProxyType += ">";

        memberVar.setAccessModifier( cppgenerate::AccessModifier::PROTECTED )
                 .setName( "m_method_" + i.name() )
                 .setType( "std::shared_ptr<DBus::MethodProxy" + methodProxyType + "> " );

        if( i.returnType() != "void" ){
            block = "return ";
        }
        block += "(*" + memberVar.name() + ")(" +  methodArguments + ");";
        newMethod.addCode( cppgenerate::CodeBlock::create()
            .addLine( block ) )
                .setAccessModifier( cppgenerate::AccessModifier::PUBLIC );

        cls->addMethod( newMethod )
            .addMemberVariable( memberVar );

        constructor->addCode( cppgenerate::CodeBlock::create()
            .addLine( "m_method_" + i.name() +
                      " = this->create_method" + methodProxyType +
                      "(\"" + newMethod.name() + "\");" ) );
    }
}

void CodeGenerator::generateProxySignals(cppgenerate::Class *cls, cppgenerate::Constructor *constructor, std::vector<SignalInfo> signals){
    for( SignalInfo si : signals ){
        cppgenerate::MemberVariable proxyMemberVar;
        cppgenerate::Method emitSignalMethod;
        cppgenerate::Method proxy_getSignalMethod;
        std::vector<cppgenerate::Argument> args = si.arguments();
        std::string templateType;
        bool argumentComma = false;
        std::string signalEmitCode;

        templateType += "<void(";
        signalEmitCode = "(*m_signal_" + si.name() + ")(";
        for( cppgenerate::Argument arg : args ){
            emitSignalMethod.addArgument( arg );

            if( argumentComma ){
                templateType += ",";
                signalEmitCode += ",";
            }

            templateType += arg.type();
            signalEmitCode += arg.name();
            argumentComma = true;
        }
        templateType += ")>";
        signalEmitCode += ");";

        proxyMemberVar.setAccessModifier( cppgenerate::AccessModifier::PROTECTED )
                 .setName( "m_signalproxy_" + si.name() )
                 .setType( "std::shared_ptr<DBus::SignalProxy" + templateType + ">" );

        proxy_getSignalMethod = cppgenerate::Method::create()
            .setAccessModifier( cppgenerate::AccessModifier::PUBLIC )
            .setReturnType( "std::shared_ptr<DBus::SignalProxy" + templateType + ">" )
            .setName( "signal_" + si.name() )
            .setCode( cppgenerate::CodeBlock::create().addLine( "return m_signalproxy_" + si.name() + ";" ) );

        emitSignalMethod.setName( si.name() )
            .setAccessModifier( cppgenerate::AccessModifier::PUBLIC )
            .setCode( cppgenerate::CodeBlock::create().addLine( signalEmitCode ) );


        cls->addMemberVariable( proxyMemberVar )
            .addMethod( proxy_getSignalMethod );

        constructor->addCode( cppgenerate::CodeBlock::create()
            .addLine( proxyMemberVar.name() +
                      " = this->create_signal" + templateType +
                      "( \"" + si.name() + "\" );" ) );

    }
}

void CodeGenerator::generateProxyProperties(cppgenerate::Class *cls, cppgenerate::Constructor *constructor, std::vector<PropertyInfo> properties){
    for( PropertyInfo pi : properties ){
        cppgenerate::MemberVariable propertyProxyMember;
        cppgenerate::Argument propertyArugment;
        cppgenerate::Method propertySetMethod;
        cppgenerate::Method propertyGetMethod;
        cppgenerate::Method propertyProxyGetMethod;
        std::string templateType;

        templateType = pi.cpptype();

        propertyProxyMember.setName( "m_property_" + pi.name() )
                .setType( "std::shared_ptr<DBus::PropertyProxy<" + templateType + ">>" );

        propertySetMethod.setName( "set" + pi.name() )
                .setCode( cppgenerate::CodeBlock::create()
                          .addLine( "m_property_" + pi.name() + "->set_value( value );") )
                .addArgument( cppgenerate::Argument::create()
                              .setName( "value" )
                              .setType( pi.cpptype() ) );

        propertyGetMethod.setName( pi.name() )
                .setCode( cppgenerate::CodeBlock::create()
                          .addLine( "return m_property_" + pi.name() + "->value();") )
                .setReturnType( pi.cpptype() );

        propertyProxyGetMethod.setName( "getProperty_" + pi.name() )
                .setReturnType( "std::shared_ptr<DBus::PropertyProxy<" + templateType + ">>" )
                .setCode( cppgenerate::CodeBlock::create().addLine( "return m_property_" + pi.name() + ";" ) );

        constructor->addCode( cppgenerate::CodeBlock::create()
                              .addLine( "m_property_" + pi.name() + " = this->create_property<" + templateType + ">( \"" + pi.name() + "\" );" ) );

        cls->addMethod( propertySetMethod )
                .addMethod( propertyGetMethod )
                .addMethod( propertyProxyGetMethod )
                .addMemberVariable( propertyProxyMember );

    }
}

void CodeGenerator::generateAdapterClasses( bool outputToFile, const std::string& output_directory ){
    std::vector<cppgenerate::Class> classes;
    cppgenerate::Class mainAdapterClass;
    cppgenerate::Constructor mainAdapterConstructor;
    cppgenerate::Method createMethod;
    std::string mainClassName = m_rootNode.cppname() + "Adapter";

    mainAdapterClass.setName( mainClassName )
          .setNamespace( m_rootNode.genNamespace() )
          .addSystemInclude( "dbus-cxx.h" )
          .addSystemInclude( "stdint.h" )
          .addSystemInclude( "string" )
          .addSystemInclude( "memory" )
          .addParentClass( "DBus::Object", cppgenerate::AccessModifier::PUBLIC, "path" );

    createMethod
        .setAccessModifier( cppgenerate::AccessModifier::PUBLIC )
        .setName( "create" )
        .setStatic( true )
        .setReturnType( "std::shared_ptr<" + mainClassName + ">" )
        .addArgument( cppgenerate::Argument::create()
          .setType( "std::shared_ptr<DBus::Connection>" )
          .setName( "connection" ) );

    cppgenerate::CodeBlock constructorBlock;
    cppgenerate::CodeBlock createBlock;
    bool comma = false;
    createBlock.addLine(  "std::shared_ptr<" + mainClassName + "> adapter = std::shared_ptr<" + mainClassName + ">( new " + mainClassName + "(" );
    for( InterfaceInfo interfaceInfo : m_rootNode.interfaces() ){
        if( interfaceInfo.name() == DBUS_CXX_PEER_INTERFACE ||
                interfaceInfo.name() == DBUSCXX_INTERFACE_INTROSPECTABLE ||
                interfaceInfo.name() == DBUS_CXX_PROPERTIES_INTERFACE ){
            // This is a standard interface, ignore code generation.
            continue;
        }

        std::string interfaceAdapterName = interfaceInfo.cppname() + "Interface";

        // For each of the interfaces, we need to add a new InterfaceAdapter parameter to our create() method.
        createMethod.addArgument( cppgenerate::Argument::create()
                                  .setName( "_" + interfaceAdapterName )
                                  .setType( "std::shared_ptr<" + interfaceAdapterName + ">" ) );
        if( comma ){
            createBlock.addLine( ", _" + interfaceAdapterName );
        }else{
            createBlock.addLine( " _" + interfaceAdapterName );
            comma = true;
        }


        // For each of the interfaces, add them to us
        mainAdapterConstructor.addArgument( cppgenerate::Argument::create()
                                            .setName( "_" + interfaceAdapterName )
                                            .setType( "std::shared_ptr<" + interfaceAdapterName + ">" ) );
        constructorBlock.addLine( "this->add_interface( _" + interfaceAdapterName + ");" );
    }

    if( comma ){
        createBlock.addLine( ", path ) );" );
    }else{
        createBlock.addLine( " path ) );" );
    }

    createBlock
            .addLine( "if( connection ){ " )
            .addLine( "  if( connection->register_object( adapter, calling_thread ) != DBus::RegistrationStatus::Success ){" )
            .addLine( "    return std::shared_ptr<" + mainClassName + ">();" )
            .addLine( "  }" )
            .addLine( "}" )
            .addLine( "return adapter;" );
    createMethod.addCode( createBlock );

    createMethod.addArgument( cppgenerate::Argument::create()
          .setType( "std::string" )
          .setName( "path" )
          .setDefaultValue( m_rootNode.name() ) );
    createMethod.addArgument( cppgenerate::Argument::create()
          .setType( "DBus::ThreadForCalling" )
          .setName( "calling_thread" )
          .setDefaultValue( "DBus::ThreadForCalling::DispatcherThread" ) );

    mainAdapterConstructor.addCode( constructorBlock );
    mainAdapterConstructor
        .addArgument( cppgenerate::Argument::create()
          .setType( "std::string" )
          .setName( "path" )
          .setDefaultValue( m_rootNode.name() ) );

    mainAdapterClass.addMethod( createMethod )
            .addConstructor( mainAdapterConstructor );

    for( InterfaceInfo interfaceInfo : m_rootNode.interfaces() ){
        if( interfaceInfo.name() == DBUS_CXX_PEER_INTERFACE ||
                interfaceInfo.name() == DBUSCXX_INTERFACE_INTROSPECTABLE ||
                interfaceInfo.name() == DBUS_CXX_PROPERTIES_INTERFACE ){
            // This is a standard interface, ignore code generation.
            continue;
        }

        cppgenerate::Class interfaceAdapter;
        cppgenerate::Constructor interfaceConstructor;
        cppgenerate::Class virtualClass;
        cppgenerate::Method interfaceCreateMethod;
        std::string interfaceAdapterName = interfaceInfo.cppname() + "Interface";

        mainAdapterClass.addLocalInclude( interfaceAdapterName + ".h" );

        interfaceAdapter.setName( interfaceAdapterName )
                .addParentClass( "DBus::Interface", cppgenerate::AccessModifier::PUBLIC, "name" )
                .setNamespace( m_rootNode.genNamespace() )
                .addSystemInclude( "dbus-cxx.h" )
                .addSystemInclude( "stdint.h" )
                .addSystemInclude( "string" )
                .addSystemInclude( "memory" )
                .addLocalInclude( interfaceInfo.cppname() + ".h" );
        interfaceConstructor.addArgument( cppgenerate::Argument::create()
                              .setName( "adaptee" )
                              .setType( interfaceInfo.cppname() + "*" ))
                .addArgument( cppgenerate::Argument::create()
                              .setName( "name" )
                              .setType( "std::string" ) )
                .setAccessModifier( cppgenerate::AccessModifier::PRIVATE );
        interfaceCreateMethod.setName( "create" )
                .addArgument( cppgenerate::Argument::create()
                  .setName( "adaptee" )
                  .setType( interfaceInfo.cppname() + "*" ))
                .addArgument( cppgenerate::Argument::create()
                  .setName( "name" )
                  .setType( "std::string" )
                  .setDefaultValue( "\"" + interfaceInfo.name() + "\"" ))
                .setReturnType( "std::shared_ptr<" + interfaceAdapterName + ">" )
                .setStatic( true )
                .setAccessModifier( cppgenerate::AccessModifier::PUBLIC )
                .addCode( cppgenerate::CodeBlock::create()
                          .addLine( "return std::shared_ptr<" + interfaceAdapterName + ">( new " + interfaceAdapterName + "( adaptee, name ) );" ));

        interfaceAdapter.addMethod( interfaceCreateMethod );
        virtualClass.setName( interfaceInfo.cppname() );

        generateAdapterMethods( &interfaceAdapter, &interfaceConstructor, &virtualClass, interfaceInfo.methods() );
        generateAdapterSignals( &interfaceAdapter, &interfaceConstructor, interfaceInfo.signals() );

        interfaceAdapter.addConstructor( interfaceConstructor );
        classes.push_back( interfaceAdapter );
        classes.push_back( virtualClass );
    }

    classes.push_back( mainAdapterClass );

    for( cppgenerate::Class c : classes ){
        if( outputToFile ){
            std::string headerFilename = output_directory + c.getName() + std::string( ".h" );
            std::string implFilename = output_directory + c.getName() + std::string( ".cpp" );
            std::ofstream header;
            std::ofstream impl;

            header.open( headerFilename );
            impl.open( implFilename );

            c.print( header, impl );
            std::cerr << "Generated adapter file: " << headerFilename << "\n";
            std::cerr << "Generated adapter file: " << implFilename << "\n";
        }else{
            c.printAsHeaderOnly( std::cout );
        }
    }
}

void CodeGenerator::generateAdapterMethods( cppgenerate::Class* cls,
                                cppgenerate::Constructor* constructor,
                                cppgenerate::Class* virtualClass,
                                std::vector<MethodInfo> methods ){

    for( MethodInfo mi : methods ){
        cppgenerate::Method newVirtualMethod;
        std::vector<cppgenerate::Argument> args = mi.arguments();
        std::string dbusSig;
        /* method adapter type = template args */
        std::string methodAdapterType;
        /* methodArguments = arguments to virtual method */
        std::string methodArguments;
        bool argumentComma = false;
        std::string block;
        std::string sigc_fun = "sigc::mem_fun( *adaptee, &";

        sigc_fun += virtualClass->getName() + "::" + mi.name();

        newVirtualMethod.setName( mi.name() )
                .setReturnType( mi.returnType() )
                .setAccessModifier( cppgenerate::AccessModifier::PUBLIC )
                .setPureVirtual( true );

        methodAdapterType += "<" + mi.returnType();
        methodAdapterType += "(";
        for( cppgenerate::Argument arg : args ){
            if( argumentComma ){
                methodArguments += ",";
                methodAdapterType += ",";
            }

            argumentComma = true;

            newVirtualMethod.addArgument( arg );
            methodArguments += arg.name();
            methodAdapterType += arg.type();
        }
        methodAdapterType += ")>";

        constructor->addCode( cppgenerate::CodeBlock::create()
            .addLine( "this->create_method" + methodAdapterType +
                      "(\"" + newVirtualMethod.name() + "\", " + sigc_fun + "));" ) );
        virtualClass->addMethod( newVirtualMethod );

        for( std::string include : mi.includes() ){
            cls->addSystemInclude( include );
            virtualClass->addSystemInclude( include );
        }
    }

}

void CodeGenerator::generateAdapterSignals( cppgenerate::Class* cls,
                           cppgenerate::Constructor* constructor,
                             std::vector<SignalInfo> signals ){
    for( SignalInfo si : signals ){
        cppgenerate::MemberVariable adapterMemberVar;
        cppgenerate::Method getSignalMethod;
        cppgenerate::Method emitSignalMethod;
        std::vector<cppgenerate::Argument> args = si.arguments();
        std::string templateType;
        bool argumentComma = false;
        std::string signalEmitCode;

        templateType += "<void(";
        signalEmitCode = "(*m_signal_" + si.name() + ")(";
        for( cppgenerate::Argument arg : args ){
            emitSignalMethod.addArgument( arg );

            if( argumentComma ){
                templateType += ",";
                signalEmitCode += ",";
            }

            templateType += arg.type();
            signalEmitCode += arg.name();
            argumentComma = true;
        }
        templateType += ")>";
        signalEmitCode += ");";

        adapterMemberVar.setAccessModifier( cppgenerate::AccessModifier::PROTECTED )
                 .setName( "m_signal_" + si.name() )
                 .setType( "std::shared_ptr<DBus::Signal" + templateType + ">" );

        getSignalMethod = cppgenerate::Method::create()
            .setAccessModifier( cppgenerate::AccessModifier::PUBLIC )
            .setReturnType( "std::shared_ptr<DBus::Signal" + templateType + ">" )
            .setName( "signal_" + si.name() )
            .setCode( cppgenerate::CodeBlock::create().addLine( "return m_signal_" + si.name() + ";" ) );

        emitSignalMethod.setName( si.name() )
            .setAccessModifier( cppgenerate::AccessModifier::PUBLIC )
            .setCode( cppgenerate::CodeBlock::create().addLine( signalEmitCode ) );

        cls->addMemberVariable( adapterMemberVar )
                .addMethod( getSignalMethod )
                .addMethod( emitSignalMethod );

        constructor->addCode( cppgenerate::CodeBlock::create()
                              .addLine( "m_signal_" + si.name()
                                        + " = create_signal" + templateType + "( \"" + si.name() + "\" );" ) );

    }

}

std::tuple<std::string,std::vector<std::string>> CodeGenerator::getTemplateArgsFromSignature( SignatureIterator it ){
    std::string ret;
    std::vector<std::string> includes;

    do{
        TypeInfo info( it.type() );
        for( std::string include_file : info.includeFilesForType() ){
            includes.push_back( include_file );
        }

        if( ret.length() > 0 ){
            ret += ",";
        }

        /* Special case: if we have a dictionary, we will recurse twice, so don't
         * fill in anything for the cpp type.  This is also because a dict
         * looks like a{ss}, which is at first glance indistinguishable from
         * an array(given the 'a' type)
         */
        if( !it.is_dict() ){
            ret += info.cppType();
        }

        if( info.isTemplated() ){
            if( !it.is_dict() ) ret += "<";

            std::tuple<std::string,std::vector<std::string>> tmpRet =
                    getTemplateArgsFromSignature( it.recurse() );
            ret += std::get<0>( tmpRet );
            for( std::string include_file : std::get<1>( tmpRet ) ){
                includes.push_back( include_file );
            }
            if( !it.is_dict() ) ret += ">";
        }
    }while( it.next() );

    return std::tuple<std::string,std::vector<std::string>>( ret, includes );
}

void CodeGenerator::handle_node_tag( std::map<std::string,std::string>& tagAttrs ){
    cppgenerate::Class newclass;
    cppgenerate::Class newAdapterClass;
    cppgenerate::Class newAdapteeClass;
    std::string dest;
    std::string path;
    std::string gen_namespace;

    if( tagAttrs.find( "gen-namespace" ) != tagAttrs.end() ){
        gen_namespace = tagAttrs.find( "gen-namespace" )->second;
    }

    if( tagAttrs.find( "dest" ) != tagAttrs.end() ){
        dest = "\"" + tagAttrs[ "dest" ] + "\"";
    }else{
        std::cerr << "WARNING: Did not find 'dest' in XML for node \'"
                  << "node" << "\'.  Line:"
                  << XML_GetCurrentLineNumber( m_parser )
                  << std::endl;
    }

    if( tagAttrs.find( "name" ) != tagAttrs.end() ){
        path = "\"" + tagAttrs[ "name" ] + "\"";
    }else if( tagAttrs.find( "path" ) != tagAttrs.end() ){
        path = "\"" + tagAttrs[ "path" ] + "\"";
    }else{
        std::cerr << "WARNING: Did not find 'path' or 'name' in xml for node \'"
                  << "node" << "\'.  Line:"
                  << XML_GetCurrentLineNumber( m_parser )
                  << std::endl;
    }

    if( tagAttrs.find( "cppname" ) != tagAttrs.end() ){
        std::string cppname = tagAttrs[ "cppname" ];
        std::replace( cppname.begin(), cppname.end(), '.', '_' );
        newclass.setName( cppname + "Proxy" );
        newAdapterClass.setName( cppname + "Adapter" );
        newAdapteeClass.setName( cppname + "Adaptee" );

        m_rootNode = NodeInfo( path );
        m_rootNode.setCppname( cppname );
    }else{
        m_rootNode = NodeInfo( path );
        m_rootNode.setCppname( "NONAME" );
        newclass.setName( "NONAME_Proxy" );
        newAdapterClass.setName( "NONAME_Adapter" );
        newAdapteeClass.setName( "NONAME_Adaptee" );
    }

    m_rootNode.setDest( dest );
    m_rootNode.setNamespace( gen_namespace );
}

void CodeGenerator::handle_method_tag( std::map<std::string,std::string>& tagAttrs ){
    m_argNum = 0;
    if( tagAttrs.find( "name" ) == tagAttrs.end() ){
        std::cerr << "ERROR: No name for method found; ignoring.  Line:"
                  << XML_GetCurrentLineNumber( m_parser ) << "\n";
        return;
    }

    std::string methodName = tagAttrs[ "name" ];
    std::replace( methodName.begin(), methodName.end(), '.', '_' );

    m_currentMethodInfo = MethodInfo( methodName );
}

void CodeGenerator::handle_signal_tag( std::map<std::string,std::string>& tagAttrs ){
    m_argNum = 0;
    if( tagAttrs.find( "name" ) == tagAttrs.end() ){
        std::cerr << "ERROR: No name for signal found, ignoring.  Line:"
                  << XML_GetCurrentLineNumber( m_parser ) << "\n";
        return;
    }

    std::string signalName = tagAttrs[ "name" ];
    std::replace( signalName.begin(), signalName.end(), '.', '_' );

    m_currentSignal = SignalInfo( signalName );
}

void CodeGenerator::handle_arg_tag( std::string parentElement, std::map<std::string,std::string>& tagAttrs ){
    cppgenerate::Argument arg;

    if( tagAttrs.find( "direction" ) == tagAttrs.end() &&
        parentElement == "method" ){
        std::cerr << "WARNING: No direction for arg found(assuming in).  Line:"
                  << XML_GetCurrentLineNumber( m_parser ) << "\n";
    }

    DBus::Signature signature( tagAttrs[ "type" ] );

    DBus::Signature::iterator it = signature.begin();
    std::tuple<std::string,std::vector<std::string>> types = getTemplateArgsFromSignature( it );

    arg.setType( std::get<0>( types ) );
    if( tagAttrs[ "name" ].length() == 0 ){
        char buffer[10];
        snprintf( buffer, 10, "arg%d", m_argNum );
        arg.setName( std::string( buffer ) );
    }else{
        std::string argName = tagAttrs[ "name" ];
        if( argName == "interface" ){
            // 'interface' is invalid due to us #defining it for Windows support
            argName = "_interface";
        }
        arg.setName( argName );
    }

    if( tagAttrs[ "direction" ] == "out" && parentElement == "method" ){
        m_currentMethodInfo.addReturnValue( std::get<0>( types ) );
        m_currentMethodInfo.addIncludes( std::get<1>( types ) );
    }else if( parentElement == "method" ){
        m_currentMethodInfo.addArgument( arg );
        m_currentMethodInfo.addIncludes( std::get<1>( types ) );
    }else if( parentElement == "signal" ){
        m_currentSignal.addArgument( arg );
        m_currentSignal.addIncludes( std::get<1>( types ) );
    }

    m_argNum++;
}

void CodeGenerator::handle_property_tag( std::map<std::string, std::string> &tagAttrs ){
    std::string propertyName;
    std::string propertyType;
    std::string propertyAccess;
    bool hasError = false;

    if( tagAttrs[ "name" ].length() == 0 ){
        std::cerr << "ERROR: No name found for property; ignoring!  Line:"
                  << XML_GetCurrentLineNumber( m_parser ) << "\n";
        hasError = true;
    }

    if( tagAttrs[ "type" ].length() == 0 ){
        std::cerr << "ERROR: No type found for property; ignoring!  Line:"
                  << XML_GetCurrentLineNumber( m_parser ) << "\n";
        hasError = true;
    }

    if( tagAttrs[ "access" ].length() == 0 ){
        std::cerr << "ERROR: No access found for property; ignoring!  Line:"
                  << XML_GetCurrentLineNumber( m_parser ) << "\n";
        hasError = true;
    }

    if( hasError ){
        return;
    }

    propertyName = tagAttrs[ "name" ];
    propertyAccess = tagAttrs[ "access" ];

    DBus::Signature signature( tagAttrs[ "type" ] );

    DBus::Signature::iterator it = signature.begin();
    std::tuple<std::string,std::vector<std::string>> types = getTemplateArgsFromSignature( it );

    PropertyInfo pi( propertyName, std::get<0>( types ), propertyAccess );
    m_currentInterface.addPropertyInfo( pi );
}
