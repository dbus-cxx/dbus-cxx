#include "cppgenerateutils.h"

static char sanitizeChar( char c ){
    if( c >= 'A' && c <= 'z' ){
        return c;
    }else if( c >= '0' && c <= '9' ){
        return c;
    }else{
        return '_';
    }
}

static char uppercaseChar( char c ){
    if( c >= 'A' && c <= 'Z' ){
        return c;
    }else if( c >= 'a' && c <= 'z' ){
        return c - 32;
    }

    return '_';
}

std::string cppgenerate::sanitizeString( std::string input ){
    std::string newString;

    //Make sure that we only have valid chars.  A-Z, a-z, 0-9, _
    std::string::const_iterator it = input.begin();
    while( it != input.end() ){
        char c = *it;
        newString.push_back( sanitizeChar( c ) );
        it++;
    }

    return newString;
}

std::string cppgenerate::uppercase( std::string input ){
    std::string newString;
    std::string::const_iterator it = input.begin();

    while( it != input.end() ){
        char c = *it;
        newString.push_back( uppercaseChar( c ) );
        it++;
    }

    return newString;
}

void cppgenerate::insertSpaces( std::ostream& stream, int spaces ){
    for( int x = 0; x < spaces; x++ ){
        stream << " ";
    }
}
