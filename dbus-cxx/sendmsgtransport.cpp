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

#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <message.h>
#include <unistd.h>
#include <dbus-cxx-private.h>
#include <dbus-cxx/utility.h>

#include "sendmsgtransport.h"

using DBus::priv::SendmsgTransport;

static const char* LOGGER_NAME = "DBus.priv.SendmsgTransport";

class SendmsgTransport::priv_data {
public:
    priv_data( int fd ) :
        m_fd( fd ),
        m_ok( false ),
        rx_iovlen( 1024 ),
        rx_controllen( 512 ),
        tx_controllen( 512 ),
        tx_control_data( nullptr )
    {
        ::memset( &rx_data, 0, sizeof( struct msghdr ) );
        ::memset( &tx_data, 0, sizeof( struct msghdr ) );
        m_sendBuffer.reserve( 2048 );
    }

    ~priv_data(){
        free( rx_data.msg_iov[0].iov_base );
        free( rx_data.msg_control );
        free( tx_data.msg_control );
    }

    int m_fd;
    bool m_ok;
    std::vector<uint8_t> m_sendBuffer;

    struct msghdr rx_data;
    struct iovec rx_iov_data;
    int rx_iovlen;
    int rx_controllen;

    struct msghdr tx_data;
    struct iovec tx_iov_data;
    int tx_controllen;
    void* tx_control_data;
};

SendmsgTransport::SendmsgTransport( int fd, bool initialize ) :
    m_priv( std::make_unique<priv_data>( fd ) ){
    uint8_t nulbyte = 0;

    if( initialize ){
        if( ::write( m_priv->m_fd, &nulbyte, 1 ) < 0 ){
            int my_errno = errno;
            std::string errmsg = strerror( errno );
            SIMPLELOGGER_DEBUG(LOGGER_NAME, "Unable to write nul byte: " + errmsg );
            m_priv->m_ok = false;
            close( m_priv->m_fd );
            errno = my_errno;
            return;
        }
    }

    m_priv->m_ok = true;

    // Setup the RX data msghdr
    m_priv->rx_data.msg_iov = &m_priv->rx_iov_data;
    m_priv->rx_data.msg_iov[0].iov_base = ::malloc( m_priv->rx_iovlen );
    m_priv->rx_data.msg_iov[0].iov_len = m_priv->rx_iovlen;
    m_priv->rx_data.msg_iovlen = 1;
    m_priv->rx_data.msg_control = ::malloc( m_priv->rx_controllen );

    // Setup the TX data msghdr
    m_priv->tx_data.msg_iov = &m_priv->tx_iov_data;
    m_priv->tx_data.msg_iovlen = 1;
    m_priv->tx_control_data = ::malloc( m_priv->tx_controllen );
}

SendmsgTransport::~SendmsgTransport(){
    close( m_priv->m_fd );
}

std::shared_ptr<SendmsgTransport> SendmsgTransport::create( int fd, bool initialize ){
    return std::shared_ptr<SendmsgTransport>( new SendmsgTransport( fd, initialize ) );
}

ssize_t SendmsgTransport::writeMessage( std::shared_ptr<const DBus::Message> message, uint32_t serial ){
    std::ostringstream debug_str;
    struct cmsghdr* cmsg;
    const std::vector<int> filedescriptors = message->filedescriptors();
    int fd_space_needed = CMSG_SPACE( sizeof( int ) * filedescriptors.size() );
    ssize_t ret;

    m_priv->m_sendBuffer.clear();
    if( !message->serialize_to_vector( &m_priv->m_sendBuffer, serial ) ){
        return 0;
    }

    debug_str << "Going to send the following bytes: " << std::endl;
    DBus::hexdump( &m_priv->m_sendBuffer, &debug_str );
    SIMPLELOGGER_TRACE( LOGGER_NAME, debug_str.str() );

    m_priv->tx_iov_data.iov_base = m_priv->m_sendBuffer.data();
    m_priv->tx_iov_data.iov_len = m_priv->m_sendBuffer.size();
    m_priv->tx_data.msg_control = nullptr;
    m_priv->tx_data.msg_controllen = 0;

    if( m_priv->tx_controllen < fd_space_needed ){
        free( m_priv->tx_control_data );
        m_priv->tx_control_data = ::malloc( fd_space_needed );
        m_priv->tx_controllen = fd_space_needed;
    }

    /* Fill in our FD array(ancillary data) */
    if( filedescriptors.size() > 0 ){
        m_priv->tx_data.msg_control = m_priv->tx_control_data;
        m_priv->tx_data.msg_controllen = fd_space_needed;
        cmsg = CMSG_FIRSTHDR( &m_priv->tx_data );
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN( sizeof( int ) * filedescriptors.size() );

        int* data = (int*)CMSG_DATA( cmsg );
        for( int fd : filedescriptors ){
            *data = fd;
            data++;
        }
    }

    /* Now we finally send the data! */
    ret = sendmsg( m_priv->m_fd, &m_priv->tx_data, 0 );
    if( ret < 0 ){
        int my_errno = errno;
        debug_str.str("");
        debug_str.clear();

        debug_str << "Can't send message: " << strerror(my_errno);

        SIMPLELOGGER_ERROR( LOGGER_NAME, debug_str.str() );
        m_priv->m_ok = false;
    }

    return ret;
}

std::shared_ptr<DBus::Message> SendmsgTransport::readMessage(){
    ssize_t header_array_len;
    ssize_t body_len;
    ssize_t total_len;
    ssize_t num_fds;
    ssize_t ret;
    uint8_t* header_raw = static_cast<uint8_t*>( m_priv->rx_data.msg_iov[0].iov_base );
    struct cmsghdr* cmsg;
    std::vector<int> fds;

    /* Do a peek of the data to determine if our arrays are large enough or not */
    m_priv->rx_data.msg_namelen = 0;
    m_priv->rx_data.msg_iov[0].iov_len = 16;
    m_priv->rx_data.msg_controllen = 0;

    ret = recvmsg( m_priv->m_fd, &m_priv->rx_data, MSG_PEEK );
    if( ret < 0 ){
        return std::shared_ptr<DBus::Message>();
    }

    if( header_raw[0] == 'l' ){
        /* Little-endian */
        body_len = header_raw[ 7 ] << 24 |
            header_raw[ 6 ] << 16 |
            header_raw[ 5 ] << 8 |
            header_raw[ 4 ] << 0;

        header_array_len = header_raw[ 15 ] << 24 |
            header_raw[ 14 ] << 16 |
            header_raw[ 13 ] << 8 |
            header_raw[ 12 ] << 0;
    }else if( header_raw[0] == 'B' ){
        /* Big-endian */
        body_len = header_raw[ 4 ] << 24 |
            header_raw[ 5 ] << 16 |
            header_raw[ 6 ] << 8 |
            header_raw[ 7 ] << 0;

        header_array_len = header_raw[ 12 ] << 24 |
            header_raw[ 13 ] << 16 |
            header_raw[ 14 ] << 8 |
            header_raw[ 15 ] << 0;
    }else{
        m_priv->m_ok = false;
        return std::shared_ptr<DBus::Message>();
    }

    if( 0 != header_array_len % 8 ){
        header_array_len += 8 - (header_array_len % 8);
    }
    total_len = 12 + ( 4 + header_array_len ) + body_len;

    /* Check to see if our iovlen is big enough - expand if not */
    if( m_priv->rx_iovlen < total_len ){
        free( m_priv->rx_data.msg_iov[0].iov_base );
        m_priv->rx_data.msg_iov[0].iov_base = ::malloc( total_len );
        m_priv->rx_iovlen = total_len;
        header_raw = static_cast<uint8_t*>( m_priv->rx_data.msg_iov[0].iov_base );
    }

    /* Set the amount of data that we want to read to the size of the message */
    m_priv->rx_data.msg_iov[0].iov_len = total_len;
    m_priv->rx_data.msg_controllen = m_priv->rx_controllen;

    /* Do the real reading of the data */
    ret = recvmsg( m_priv->m_fd, &m_priv->rx_data, 0 );
    if( ret < 0 ){
        m_priv->m_ok = false;
        return std::shared_ptr<DBus::Message>();
    }

    SIMPLELOGGER_DEBUG( LOGGER_NAME, "Have " << m_priv->rx_data.msg_controllen << " bytes of control aftere real reading" );

    /* Need to figure out how many FDs we have to create our array */
    for( cmsg = CMSG_FIRSTHDR(&m_priv->rx_data);
        cmsg != nullptr;
        cmsg = CMSG_NXTHDR(&m_priv->rx_data, cmsg) ) {
        if( cmsg->cmsg_level == SOL_SOCKET &&
            cmsg->cmsg_type == SCM_RIGHTS ){
            /* This is our FD array */
            num_fds = CMSG_LEN( cmsg->cmsg_len ) / sizeof( int );
            SIMPLELOGGER_DEBUG( LOGGER_NAME, "Have " << num_fds << " fds to extract from CMSGHDR" );
            int* fd_array = reinterpret_cast<int*>( CMSG_DATA( cmsg ) );
            for( ssize_t current = 0; current < num_fds; current++ ){
                fds.push_back( *fd_array );
                fd_array++;
            }
        }
    }

    std::shared_ptr<DBus::Message> retmsg =
            DBus::Message::create_from_data( header_raw, m_priv->rx_data.msg_iov->iov_len, fds );

    return retmsg;
}

bool SendmsgTransport::is_valid() const{
    return m_priv->m_ok;
}

int SendmsgTransport::fd() const{
    return m_priv->m_fd;
}
