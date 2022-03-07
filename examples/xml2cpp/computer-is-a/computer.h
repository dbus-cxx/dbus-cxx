/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
 *   rvinyard@cs.nmsu.edu                                                  *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#ifndef COMPUTER_H
#define COMPUTER_H

#include <sigc++/sigc++.h>
#include <string>
#include <cstdint>

#include "calculator.h"

namespace Examples
{

  class Computer: public Calculator
  {
    public:

      typedef enum Operation {
        NONE,
        FACTORIAL,
        FIBONACCI,
        THUE_MORSE
      } Operation;

      Computer(): m_last_op(NONE) { }

      ~Computer() { }

      uint64_t compute( Operation op, uint8_t n );

      uint64_t factorial( uint8_t n );

      uint64_t fibonacci( uint8_t n );

      uint64_t thue_morse( uint8_t n );

      sigc::signal<void,std::string,uint64_t,uint8_t> signal_computation();

      sigc::signal<void,Operation> signal_op();

      Operation last_op();

    protected:

      Operation m_last_op;

      sigc::signal<void,std::string,uint64_t,uint8_t> m_signal_computation;

      sigc::signal<void,Operation> m_signal_op;

      uint64_t compute_factorial( uint8_t n );

      uint64_t compute_fibonacci( uint8_t n );

      uint64_t compute_thue_morse( uint8_t n );

  };

}

#endif
