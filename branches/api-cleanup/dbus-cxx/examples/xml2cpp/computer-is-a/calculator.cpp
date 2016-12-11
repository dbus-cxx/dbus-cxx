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

  double Calculator::add( double a, double b )
  {
    double result = a+b;
    m_signal_calculation.emit( "add", "+", result, a, b );
    return result;
  }

  double Calculator::subtract( double a, double b )
  {
    double result = a-b;
    m_signal_calculation.emit( "subtract", "-", result, a, b );
    return result;
  }

  double Calculator::multiply( double a, double b )
  {
    double result = a*b;
    m_signal_calculation.emit( "multiply", "*", result, a, b );
    return result;
  }

  double Calculator::divide( double a, double b )
  {
    double result = a/b;
    m_signal_calculation.emit( "divide", "/", result, a, b );
    return result;
  }

  double Calculator::pi()
  {
    return M_PI;
  }

  void Calculator::print_pi()
  {
    std::cout << M_PI << std::endl;
  }

  sigc::signal< void, std::string, std::string, double, double, double > Calculator::signal_calculation()
  {
    return m_signal_calculation;
  }

}
