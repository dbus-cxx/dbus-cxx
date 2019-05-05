#ifndef CPPGENERATE_ENUM_H
#define CPPGENERATE_ENUM_H

#include <string>
#include <map>

namespace cppgenerate{

/**
 * Represents an enum in the generated code
 */
class Enum{
  public:
    Enum();
    Enum( const Enum& other );

    ~Enum();

    Enum& operator=( const Enum& other );

    Enum& setName( std::string name );

    /**
     * Set if this enum should be output as enum class <name>{}.
     * Requires this to be compiled with at least C++11
     */
    Enum& setIsEnumClass( bool isClass );

    /**
     * Add a name to the enum with the specified value.
     */
    Enum& addEnumValue( std::string name, int value );

    /**
     * Add the type to the enum, without specifying the value.
     * The generated code will pick the best value to use.
     */
    Enum& addEnumValue( std::string name );

    /**
     * Generate a method that will convert from an integer
     * to the enum type.
     */
    Enum& generateFromValue( bool generatedFromValue );

    /**
     * Generate a method that will convert from a string value
     * to the enum type.
     */
    Enum& generateFromString( bool generateFromString );

    /**
     * Set the default value of the enum, if the conversion from
     * int/string fails.  If this is not set, the first value in the enum
     * will be returned.  Note: The value must have been previously added
     * via a call to addEnumValue, otherwise this method will fail.
     */
    Enum& setDefaultValue( std::string name );

    /**
     * Generate a method that will turn the enum value into an int.
     */
    Enum& generateToInt( bool generateToInt );

    /**
     * Generate a method that will turn the enum value into a string.
     */
    Enum& generateToString( bool generateToString );

    /**
     * Set the namespace for this enum.  Ignored if this enum is
     * part of a class.
     */
    Enum& setNamespace( std::string namespaceName );

    /**
     * Get the name of this enum.  The name will have been sanitized.
     */
    const std::string getName() const;

    /**
     * Get the map of all of the enum names to the integer value
     */
    const std::map<std::string, int> getEnumNameToValues() const;

  private:
    std::string m_name;
    bool m_isEnumClass;
    std::map<std::string, int> m_enumNameToValue;
    bool m_generateFromValue;
    bool m_generateFromString;
    std::string m_defaultValue;
    bool m_generateToInt;
    bool m_generateToString;
    std::string m_namespace;
};

}

#endif /* CPPGENERATE_ENUM_H */
