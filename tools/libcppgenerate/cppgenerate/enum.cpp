#include "enum.h"
#include "cppgenerateutils.h"

using cppgenerate::Enum;

Enum::Enum() :
    m_isEnumClass( false )
{}

Enum::Enum( const Enum& other ){
}

Enum::~Enum(){
}

Enum& Enum::setName( std::string name ){
    m_name = cppgenerate::sanitizeString( name );
    return *this;
}

const std::string Enum::getName() const {
    return m_name;
}

Enum& Enum::setIsEnumClass( bool isClass ){
    m_isEnumClass = isClass;
    return *this;
}

Enum& Enum::addEnumValue( std::string name, int value ){
    m_enumNameToValue[ name ] = value;
    return *this;
}

Enum& Enum::addEnumValue( std::string name ){
    int maxValue = 0;
    std::map<std::string,int>::const_iterator it = m_enumNameToValue.begin();

    while( it != m_enumNameToValue.end() ){
        int currentValue = (*it).second;
        if( currentValue > maxValue ){
            maxValue = currentValue;
        }
        it++;
    }

    return addEnumValue( name, maxValue + 1 );
}

const std::map<std::string,int> Enum::getEnumNameToValues() const{
    return m_enumNameToValue;
}

Enum& Enum::generateFromValue( bool generateFromValue ){
    m_generateFromValue = generateFromValue;
    return *this;
}

Enum& Enum::generateFromString( bool generateFromString ){
    m_generateFromString = generateFromString;
    return *this;
}

Enum& Enum::setDefaultValue( std::string name ){
    std::map<std::string,int>::const_iterator it = m_enumNameToValue.begin();
    bool found = false;
    while( it != m_enumNameToValue.end() ){
        if( (*it).first == name ){
            found = true;
            break;
        }
    }

    if( found ){
        m_defaultValue = name;
    }
    return *this;
}

Enum& Enum::generateToInt( bool generateToInt ){
    m_generateToInt = generateToInt;
    return *this;
}

Enum& Enum::generateToString( bool generateToString ){
    m_generateToString = generateToString;
    return *this;
}

Enum& Enum::setNamespace( std::string namespaceName ){
    m_namespace = namespaceName;
    return *this;
}
