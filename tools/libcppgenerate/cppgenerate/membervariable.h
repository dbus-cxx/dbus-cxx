#ifndef CPPGENERATE_MEMBERVARIABLE_H
#define CPPGENERATE_MEMBERVARIABLE_H

#include "variable.h"
#include "codeblock.h"
#include "accessmodifier.h"

namespace cppgenerate{

/**
 * A MemberVariable is a special type of variable that is
 * a member of a class.
 */
class MemberVariable : public Variable {
  public:
    MemberVariable();

    ~MemberVariable();

    MemberVariable( const MemberVariable& other );

    MemberVariable& operator=( const MemberVariable& other );

    MemberVariable& setAccessModifier( AccessModifier modifier );

    MemberVariable& generateSetter( bool generateSetter );

    MemberVariable& generateGetter( bool generateGetter );

    /**
     * Set to true if this member variable is a QProperty.  This only has
     * an effect if the class that has this member variable is a subclass of
     * QObject.
     */
    MemberVariable& isQProperty( bool isQproperty );

    /**
     * If this member variable is a QProperty, set the signal name that will
     * be emitted if the property value is changed.  If this is not set,
     * a signal will not be emitted when the value is changed.
     */
    MemberVariable& setChangedSignalName( std::string signalName );

    static MemberVariable create();

    /**
     * Print the member variable to the output stream
     */
    void print( std::ostream& stream, int indent, bool withAccessModifier ) const;

    AccessModifier accessModifier() const;

  private:
    AccessModifier m_access;
    bool m_generateSetter;
    bool m_generateGetter;
    bool m_isQProperty;
    std::string m_changedSignalName;
};

}

#endif /* CPPGENERATE_MEMBERVARIABLE_H */
