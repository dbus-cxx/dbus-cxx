/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
 *   rvinyard@cs.nmsu.edu                                                  *
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

#include "computer.h"

#include <iostream>
#include <cmath>

namespace Examples
{

  uint64_t Computer::compute( Operation op, uint8_t n )
  {
    switch (op)
    {
      case FACTORIAL:  return factorial(n);
      case FIBONACCI:  return fibonacci(n);
      case THUE_MORSE: return thue_morse(n);
    }
    throw(-1);
  }

  uint64_t Computer::factorial( uint8_t n )
  {
    uint64_t result = compute_factorial( n );
    m_last_op = FACTORIAL;
    m_signal_computation.emit( "factorial", result, n );
    m_signal_op.emit(FACTORIAL);
    return result;
  }

  uint64_t Computer::fibonacci( uint8_t n )
  {
    uint64_t result = compute_fibonacci( n );
    m_last_op = FIBONACCI;
    m_signal_computation.emit( "Fibonacci", result, n );
    m_signal_op.emit(FIBONACCI);
    return result;
  }

  uint64_t Computer::thue_morse( uint8_t n )
  {
    uint64_t result = compute_thue_morse( n );
    m_last_op = THUE_MORSE;
    m_signal_computation.emit( "Thue-Morse", result, n );
    m_signal_op.emit(THUE_MORSE);
    return result;
  }

  Computer::Operation Computer::last_op()
  {
    return m_last_op;
  }

  sigc::signal< void, std::string, uint64_t, uint8_t > Computer::signal_computation()
  {
    return m_signal_computation;
  }

  sigc::signal<void,Computer::Operation> Computer::signal_op()
  {
    return m_signal_op;
  }

  uint64_t Computer::compute_factorial( uint8_t n )
  {
    if ( n == 0 ) return 1;
    return n * compute_factorial( n-1 );
  }

  uint64_t Computer::compute_fibonacci( uint8_t n )
  {
    if ( n == 0 ) return 0;
    if ( n == 1 ) return 1;
    return compute_fibonacci( n-1 ) + compute_fibonacci( n-2 );
  }

  uint64_t Computer::compute_thue_morse( uint8_t n )
  {
    if ( n == 0 ) return 0;
    if ( n > 6 ) return 0;
    uint64_t result, prev, neg, current, mask;
    prev = compute_thue_morse( n-1 );
    mask = ~( 0xFFFFFFFFFFFFFFFFULL << (int)pow(2,n-1) );
    neg = ( ~prev ) & mask;
    prev = prev << (int)pow(2,n-1);
    result = prev | neg;
    return result;
  }

}


