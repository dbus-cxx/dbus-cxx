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
#ifndef DBUSCXX_VARIANTAPPENDITERATOR_H
#define DBUSCXX_VARIANTAPPENDITERATOR_H

#include <vector>
#include <string>
#include <dbus-cxx/signature.h>
#include <dbus-cxx/path.h>
#include <dbus-cxx/filedescriptor.h>

namespace DBus {

class Variant;

/**
 * Used to append a single, complete value to a Variant.
 *
 * Note that all `operator<<` intentionally return void, as a
 * Variant can only hold a single complete type and cannot be chained.
 *
 * This is essentially a variant-specific version of MessageAppendIterator
 */
class VariantAppendIterator {
private:
    VariantAppendIterator( Variant* variant, ContainerType t );
public:
    VariantAppendIterator( Variant* variant );
    ~VariantAppendIterator();

    void operator<<( const bool& v );
    void operator<<( const uint8_t& v );
    void operator<<( const int16_t& v );
    void operator<<( const uint16_t& v );
    void operator<<( const int32_t& v );
    void operator<<( const uint32_t& v );
    void operator<<( const int64_t& v );
    void operator<<( const uint64_t& v );
    void operator<<( const double& v );
    void operator<<( const char* v );
    void operator<<( const std::string& v );
    void operator<<( const Signature& v );
    void operator<<( const Path& v );

    template <typename T>
    void operator<<( const std::vector<T>& v){
        open_container( ContainerType::ARRAY, DBus::signature( v ) );
        VariantAppendIterator* sub = sub_iterator();
        for( T t : v ){
            (*sub) << t;
        }
        close_container();
    }

    template <typename Key, typename Data>
    void operator<<( const std::map<Key,Data>& dictionary ){

    }

    template <typename... T>
    void operator<<( const std::tuple<T...>& tup){

    }

private:
    bool open_container( ContainerType t, const std::string& contained_signature );

    bool close_container( );

    VariantAppendIterator* sub_iterator();

private:
    class priv_data;

    std::shared_ptr<priv_data> m_priv;
};

} /* namespace DBus */

#endif /* DBUSCXX_VARIANTAPPENDITERATOR_H */
