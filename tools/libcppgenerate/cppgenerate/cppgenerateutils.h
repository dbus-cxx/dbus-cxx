#ifndef CPPGENERATE_UTILS_H
#define CPPGENERATE_UTILS_H

#include <string>
#include <iostream>

namespace cppgenerate{

std::string sanitizeString( std::string input );

std::string uppercase( std::string uppercase );

void insertSpaces( std::ostream& stream, int spaces );

}

#endif
