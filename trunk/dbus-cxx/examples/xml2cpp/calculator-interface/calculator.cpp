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

#include "calculator.h"

#include <iostream>
#include <cmath>

namespace Examples
{

  double Calculator::add( double a, double b ) const
  {
    double result = a+b;
    m_signal_calculation.emit( "add", "+", result, a, b );
    return result;
  }

  double Calculator::subtract( double a, double b ) const
  {
    double result = a-b;
    m_signal_calculation.emit( "subtract", "-", result, a, b );
    return result;
  }

  double Calculator::multiply( double a, double b ) const
  {
    double result = a*b;
    m_signal_calculation.emit( "multiply", "*", result, a, b );
    return result;
  }

  double Calculator::divide( double a, double b ) const
  {
    double result = a/b;
    m_signal_calculation.emit( "divide", "/", result, a, b );
    return result;
  }

  sigc::signal< void, std::string, std::string, double, double, double >& Calculator::signal_calculation()
  {
    return m_signal_calculation;
  }

  uint64_t Calculator::factorial( uint8_t n ) const
  {
    uint64_t result = compute_factorial( n );
    m_signal_computation.emit( "factorial", result, n );
    return result;
  }

  uint64_t Calculator::fibonacci( uint8_t n ) const
  {
    uint64_t result = compute_fibonacci( n );
    m_signal_computation.emit( "Fibonacci", result, n );
    return result;
  }

  uint64_t Calculator::thue_morse( uint8_t n ) const
  {
    uint64_t result = compute_thue_morse( n );
    m_signal_computation.emit( "Thue-Morse", result, n );
    return result;
  }

  sigc::signal< void, std::string, uint64_t, uint8_t >& Calculator::signal_computation()
  {
    return m_signal_computation;
  }

  double Calculator::pi() const
  {
    return M_PI;
  }

  void Calculator::print_pi() const
  {
    std::cout << M_PI << std::endl;
  }

  uint64_t Calculator::compute_factorial( uint8_t n ) const
  {
    if ( n == 0 ) return 1;
    return n * compute_factorial( n-1 );
  }

  uint64_t Calculator::compute_fibonacci( uint8_t n ) const
  {
    if ( n == 0 ) return 0;
    if ( n == 1 ) return 1;
    return compute_fibonacci( n-1 ) + compute_fibonacci( n-2 );
  }

  uint64_t Calculator::compute_thue_morse( uint8_t n ) const
  {
    if ( n == 0 ) return 0;
    if ( n > 6 ) return 0;
    uint64_t result, prev, neg, mask;
    prev = compute_thue_morse( n-1 );
    mask = ~( 0xFFFFFFFFFFFFFFFFULL << (int)pow(2,n-1) );
    neg = ( ~prev ) & mask;
    prev = prev << (int)pow(2,n-1);
    result = prev | neg;
    return result;
  }

}
