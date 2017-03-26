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

#include "calculator_interface.h"

namespace Examples
{

  class Calculator: public CalculatorInterface
  {
    public:

      typedef DBusCxxPointer<Calculator> pointer;

      Calculator() { }

      virtual ~Calculator() { }

      virtual double add( double a, double b ) const;

      virtual double subtract( double a, double b ) const;

      virtual double multiply( double a, double b ) const;

      virtual double divide( double a, double b ) const;

      virtual sigc::signal<void,std::string,std::string,double,double,double>& signal_calculation();

      virtual uint64_t factorial( uint8_t n ) const;

      virtual uint64_t fibonacci( uint8_t n ) const;

      virtual uint64_t thue_morse( uint8_t n ) const;

      virtual sigc::signal<void,std::string,uint64_t,uint8_t>& signal_computation();

      virtual double pi() const;

      virtual void print_pi() const;

      virtual std::string name() const { return m_name; }

      virtual void set_name( const std::string& name ) { m_name = name; }

    protected:

      sigc::signal<void,std::string,std::string,double,double,double> m_signal_calculation;

      sigc::signal<void,std::string,uint64_t,uint8_t> m_signal_computation;

      uint64_t compute_factorial( uint8_t n ) const;

      uint64_t compute_fibonacci( uint8_t n ) const;

      uint64_t compute_thue_morse( uint8_t n ) const;

      std::string m_name;

  };

}

#endif
