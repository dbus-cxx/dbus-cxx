#ifndef DBUSCXX_MARSHALING_H
#define DBUSCXX_MARSHALING_H

#include <stdint.h>
#include <vector>
#include <string>
#include <map>
#include <dbus-cxx/variant.h>
#include <dbus-cxx/path.h>
#include <dbus-cxx/signature.h>

namespace DBus{

enum class Endianess {
	Little,
	Big,
};

/**
 * Implements the marshaling/demarshaling algorithms on a given vector of data.
 */
class Marshaling {
public:
	Marshaling();

	/**
	 * Create a new Marshaling class that operates on the given
	 * vector of data, with the given endianess.
	 */
	Marshaling( std::vector<uint8_t>* data, Endianess endian );

	void setData( std::vector<uint8_t>* data );
	void setEndianess( Endianess endian );

	void marshal( bool v );
	void marshal( uint8_t v );
	void marshal( int16_t v );
	void marshal( uint16_t v );
	void marshal( int32_t v );
	void marshal( uint32_t v );
	void marshal( int64_t v );
	void marshal( uint64_t v );
	void marshal( double v );
	void marshal( std::string v );
	void marshal( Path v );
	void marshal( Signature v );
	void marshal( const Variant& v );

    void align( int alignment );

    void marshalAtOffset( uint32_t offset, uint32_t value );

private:
	void marshalShortBig( uint16_t toMarshal );
	void marshalIntBig( uint32_t toMarshal );
	void marshalLongBig( uint64_t toMarshal );
	void marshalShortLittle( uint16_t toMarshal );
	void marshalIntLittle( uint32_t toMarshal );
	void marshalLongLittle( uint64_t toMarshal );

private:
	std::vector<uint8_t>* m_data;
    Endianess m_endian;
	int m_dataPos;
};

}

#endif
