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
#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <sigc++/sigc++.h>
#include <string>
#include <cstdint>

namespace Examples
{

  class Calculator
  {
    public:

      Calculator() { }

      ~Calculator() { }

      double add( double a, double b );

      double subtract( double a, double b );

      double multiply( double a, double b );

      double divide( double a, double b );

      double pi();

      void print_pi();

      sigc::signal<void,std::string,std::string,double,double,double> signal_calculation();

    protected:

      sigc::signal<void,std::string,std::string,double,double,double> m_signal_calculation;

  };

}

#endif
