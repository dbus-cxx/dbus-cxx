// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
 *   rvinyard@cs.nmsu.edu                                                  *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#include "path.h"

#define DBUSCXX_VALID_PATH_CHARACTERS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_/"

namespace DBus {

Path::Path() {
}

Path::Path( const std::string& s, size_type pos, size_type n ):
    std::string( s, pos, n ) {
}

Path::Path( const char* s ):
    std::string( s ) {
}

Path::Path( const char* s, size_type n ):
    std::string( s, n ) {
}

Path::Path( size_type n, char c ):
    std::string( n, c ) {
}

Path::~Path() {
}

Path::operator bool() const {
    return this->is_valid();
}

bool Path::is_valid() const {
    if( this->empty() ) { return false; }

    if( this->operator[]( 0 ) != '/' ) { return false; }

    if( this->find_first_not_of( DBUSCXX_VALID_PATH_CHARACTERS ) != npos ) { return false; }

    if( this->find( "//" ) != npos ) { return false; }

    char last = *( this->rbegin() );

    if( this->size() > 1 && last == '/' ) { return false; }

    return true;
}

std::vector<std::string> Path::decomposed() const {
    if( !this->is_valid() ) { return std::vector<std::string>(); }

    std::vector<std::string> decomposed;
    std::string current;
    const_iterator i;

    if( this->size() == 1 ) {
        decomposed.push_back( current );
        return decomposed;
    }

    // iterate through character by character looking for delimeters
    for( i = this->begin() + 1; i != this->end(); i++ ) {
        // if we found a delimiter add the current string and reset
        // for the next segment
        if( *i == '/' ) {
            decomposed.push_back( current );
            current.clear();
        }
        // otherwise add the character to the current string
        else {
            current += *i;
        }
    }

    decomposed.push_back( current );

    return decomposed;
}

bool Path::append_element( const std::string& element ) {
    // Check for empty elements, elements with invalid characters or with
    // invalid sequences.
    if( element.empty() ) { return false; }

    if( element.find_first_not_of( DBUSCXX_VALID_PATH_CHARACTERS ) != npos ) { return false; }

    if( element.find( "//" ) != npos ) { return false; }

    if( element.size() == 1 && element[0] == '/' ) { return false; }

    // Do we have a leading /? If not we'll put one in place
    if( element[0] != '/' ) { this->append( "/" ); }

    // Do we have a trailing '/'? If so, we won't copy it.
    if( *( element.rbegin() ) == '/' ) {
        this->append( element, 0, element.size() - 1 );
    }
    // Otherwise we can simply append the element
    else {
        this->append( element );
    }

    return true;
}

}




