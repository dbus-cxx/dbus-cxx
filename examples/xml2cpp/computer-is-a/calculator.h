/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
 *   rvinyard@cs.nmsu.edu                                                  *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
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
