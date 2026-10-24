// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#include "validator.h"

using DBus::Validator;

static bool is_allowable_character( char c ) {
    if( c >= 'A' && c <= 'Z' ) {
        return true;
    }

    if( c >= 'a' && c <= 'z' ) {
        return true;
    }

    if( c >= '0' && c <= '9' ) {
        return true;
    }

    if( c == '_' ) {
        return true;
    }

    return false;
}


bool Validator::validate_bus_name( std::string busname ) {
    char previousChar = '\0';
    int numElements = 1;
    std::string::iterator it = busname.begin();
    bool isUnique = false;

    if( busname.size() > 255 || busname.empty() ) {
        return false;
    }

    if( *it == '.' ) {
        // A period is not allowed as the first character
        return false;
    }

    if( *it == ':' ) {
        // A colon is allowed as the first character
        it++;
        isUnique = true;
    }

    for( ; it < busname.end(); it++ ) {
        if( *it != '.' && !is_allowable_character( *it ) ) { return false; }

        if( previousChar == '.' && is_allowable_character( *it ) ) {
            if( !isUnique && std::isdigit( *it ) ) {
                return false;
            }

            numElements++;
        }

        previousChar = *it;
    }

    return numElements >= 2;
}

bool Validator::validate_interface_name( std::string interfacename ) {
    char previousChar = '\0';
    int numElements = 1;
    std::string::iterator it = interfacename.begin();

    if( interfacename.size() > 255 || interfacename.empty() ) {
        return false;
    }

    if( *it == '.' ) {
        // A period is not allowed as the first character
        return false;
    }

    for( ; it < interfacename.end(); it++ ) {
        if( *it != '.' && !is_allowable_character( *it ) ) { return false; }

        if( previousChar == '.' && is_allowable_character( *it ) ) {
            if( std::isdigit( *it ) ) {
                return false;
            }

            numElements++;
        }

        previousChar = *it;
    }

    return numElements >= 2;
}

bool Validator::validate_member_name( std::string name ) {
    if( name.size() > 255 || name.empty() ) {
        return false;
    }

    // Member names must not begin with a digit
    if( name.at( 0 ) < '0' ||
        name.at( 0 ) > '9' ) {
        return false;
    }

    for( const char& c : name ) {
        if( !is_allowable_character( c ) ) {
            return false;
        }
    }

    return true;
}

bool Validator::validate_error_name( std::string errorname ) {
    return validate_interface_name( errorname );
}

bool Validator::message_is_small_enough( const std::vector<uint8_t>* data ) {
    return data->size() < maximum_message_size();
}

/*
 * Limits from the DBus specification
 */
static const std::string::size_type MAX_SIGNATURE_LENGTH = 255;
static const int MAX_ARRAY_DEPTH = 32;
static const int MAX_STRUCT_DEPTH = 32;

static bool is_basic_type_code( char c ) {
    switch( c ) {
    case 'y': case 'b': case 'n': case 'q': case 'i': case 'u':
    case 'x': case 't': case 'd': case 's': case 'o': case 'g':
    case 'h':
        return true;
    }

    return false;
}

/**
 * Check that a single complete type starts at pos, following the rules in
 * the DBus specification.  On success, pos is left after the type.
 */
static bool validate_single_complete_type( const std::string& sig,
    std::string::size_type& pos,
    int array_depth,
    int struct_depth ) {
    if( pos >= sig.size() ) {
        return false;
    }

    char c = sig[ pos ];

    if( is_basic_type_code( c ) || c == 'v' ) {
        pos++;
        return true;
    }

    if( c == 'a' ) {
        if( ++array_depth > MAX_ARRAY_DEPTH ) {
            return false;
        }

        pos++;

        if( pos < sig.size() && sig[ pos ] == '{' ) {
            // Dict entries may only appear as the element type of an array,
            // and must have exactly two types, the first of which is basic
            if( ++struct_depth > MAX_STRUCT_DEPTH ) {
                return false;
            }

            pos++;

            if( pos >= sig.size() || !is_basic_type_code( sig[ pos ] ) ) {
                return false;
            }

            pos++;

            if( !validate_single_complete_type( sig, pos, array_depth, struct_depth ) ) {
                return false;
            }

            if( pos >= sig.size() || sig[ pos ] != '}' ) {
                return false;
            }

            pos++;
            return true;
        }

        return validate_single_complete_type( sig, pos, array_depth, struct_depth );
    }

    if( c == '(' ) {
        if( ++struct_depth > MAX_STRUCT_DEPTH ) {
            return false;
        }

        pos++;

        // Structs must have at least one member
        do {
            if( !validate_single_complete_type( sig, pos, array_depth, struct_depth ) ) {
                return false;
            }
        } while( pos < sig.size() && sig[ pos ] != ')' );

        if( pos >= sig.size() ) {
            return false;
        }

        pos++;
        return true;
    }

    return false;
}

bool Validator::validate_signature( const std::string& sig ) {
    std::string::size_type pos = 0;

    if( sig.size() > MAX_SIGNATURE_LENGTH ) {
        return false;
    }

    while( pos < sig.size() ) {
        if( !validate_single_complete_type( sig, pos, 0, 0 ) ) {
            return false;
        }
    }

    return true;
}
