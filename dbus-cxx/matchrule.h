/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
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
#ifndef DBUSCXX_MATCH_RULE_H
#define DBUSCXX_MATCH_RULE_H

#include <memory>
#include "dbus-cxx-config.h"

namespace DBus {

class MatchRuleBuilder;
class MatchRuleData;

/**
 * Immutable class that represents a match rule for DBus.
 */
class MatchRule {
protected:
    MatchRule( std::string type, const std::shared_ptr<MatchRuleData> );

public:
    std::string match_rule() const;

    std::string path() const;

    std::string dbus_interface() const;

    std::string member() const;

private:
    DBUS_CXX_PROPAGATE_CONST( std::shared_ptr<MatchRuleData> ) m_priv;

    friend class MatchRuleBuilder;
};

/**
 * A special MatchRule for signals.
 */
class SignalMatchRule : public MatchRule {
private:
    SignalMatchRule( std::shared_ptr<MatchRuleData> );

    friend class MatchRuleBuilder;
};

class MethodCallMatchRule : public MatchRule {
private:
    MethodCallMatchRule( std::shared_ptr<MatchRuleData> );

    friend class MatchRuleBuilder;
};

class MethodReturnMatchRule : public MatchRule {
private:
    MethodReturnMatchRule( std::shared_ptr<MatchRuleData> );

    friend class MatchRuleBuilder;
};

class ErrorMatchRule : public MatchRule {
private:
    ErrorMatchRule( std::shared_ptr<MatchRuleData> );

    friend class MatchRuleBuilder;
};

/**
 * A builder to create match rules.  When you are done building the match rule,
 * call the appropriate method(asSignalMatch, asMethodCallMatch, etc).
 */
class MatchRuleBuilder {
protected:
    MatchRuleBuilder();

public:
    MatchRuleBuilder& set_path( const std::string& path );

    MatchRuleBuilder& set_interface( const std::string& interface_name );

    MatchRuleBuilder& set_member( const std::string& member );

    MatchRuleBuilder& set_sender( const std::string& sender );

    MatchRuleBuilder& set_destination( const std::string& destination );

    SignalMatchRule as_signal_match();

    MethodCallMatchRule as_method_call_match();

    MethodReturnMatchRule as_method_return_match();

    ErrorMatchRule as_error_match();

    static MatchRuleBuilder create();

private:
    std::shared_ptr<MatchRuleData> m_priv;
};



} /* namespace DBus */

#endif /* DBUSCXX_MATCH_RULE_H */
