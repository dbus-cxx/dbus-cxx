/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
 *   rvinyard@cs.nmsu.edu                                                  *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#ifndef CALCULATOR_INTERFACE_H
#define CALCULATOR_INTERFACE_H

#include <sigc++/sigc++.h>
#include <string>
#include <cstdint>

#include <memory>

namespace Examples
{

  class CalculatorInterface
  {
    public:

      typedef std::shared_ptr<CalculatorInterface> pointer;

      CalculatorInterface() { }

      virtual ~CalculatorInterface() { }

      virtual double add( double a, double b ) const = 0;

      virtual double subtract( double a, double b ) const = 0;

      virtual double multiply( double a, double b ) const = 0;

      virtual double divide( double a, double b ) const = 0;

      virtual sigc::signal<void,std::string,std::string,double,double,double>& signal_calculation() = 0;

      virtual uint64_t factorial( uint8_t n ) const = 0;

      virtual uint64_t fibonacci( uint8_t n ) const = 0;

      virtual uint64_t thue_morse( uint8_t n ) const = 0;

      virtual sigc::signal<void,std::string,uint64_t,uint8_t>& signal_computation() = 0;

      virtual double pi() const = 0;

      virtual void print_pi() const = 0;

      virtual std::string name() const = 0;

      virtual void set_name( const std::string& name ) = 0;

  };

}

#endif
