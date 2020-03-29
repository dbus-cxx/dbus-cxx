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
#ifndef DBUSCXX_SASL_H
#define DBUSCXX_SASL_H

#include <tuple>
#include <vector>
#include <stdint.h>

namespace DBus {
namespace priv{

/**
 * Implements the authentication routines for connection to the bus
 */
class SASL {
public:
    /**
     * Create a new SASL authenticator
     *
     * @param fd The FD to authenticate on
     * @param negotiateFDPassing True if we should try to negotiate
     * FD passing, false otherwise
     */
    SASL( int fd, bool negotiateFDPassing );

    /**
     * Perform the authentication with the server.
     *
     * @return A tuple containing the following:
     * - bool Success of authentication
     * - bool If this supports FD passing
     * - vector The GUID of the server
     */
    std::tuple<bool,bool,std::vector<uint8_t>> authenticate();

private:
    int write_data_with_newline( std::string data );
    std::string read_data();
    std::string encode_as_hex( int num );
    std::vector<uint8_t> hex_to_vector( std::string hexData );

private:
    int m_fd;
    bool m_negotiateFDpassing;
};

} /* namespace priv */
} /* namespace DBus */

#endif /* DBUSCXX_SASL_H */
