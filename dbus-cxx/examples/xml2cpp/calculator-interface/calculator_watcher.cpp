/***************************************************************************
 *   Copyright (C) 2009,2010 by Rick L. Vinyard, Jr.                       *
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

#include "calculator_proxy.h"

#include <unistd.h>

class Watcher {
  public:
    Watcher()  { }
    ~Watcher() { }

    void watch( Examples::CalculatorInterface::pointer calc )
    {
      calc->signal_calculation().connect( sigc::bind( sigc::mem_fun(*this,&Watcher::print_calculation), calc ) );
      calc->signal_computation().connect( sigc::bind( sigc::mem_fun(*this,&Watcher::print_computation), calc ) );
    }
    
    void print_calculation(std::string op, std::string opsym, double result, double param1, double param2, Examples::CalculatorInterface::pointer ci)
    {
      std::cout << ci->name() << ": " << param1 << " " << opsym << " " << param2 << " = " << result << std::endl;
    }

    void print_computation(std::string op, uint64_t result, uint8_t n, Examples::CalculatorInterface::pointer ci)
    {
      std::cout << ci->name() << ": " << op << "(" << (int)n << ") = " << result << std::endl;
    }
};

class Recalculator: public Examples::Calculator {
  public:

    typedef DBusCxxPointer<CalculatorInterface> pointer;

    Recalculator(Examples::CalculatorInterface::pointer base)
    {
      if ( base ) base->signal_calculation().connect( sigc::mem_fun(*this, &Recalculator::on_calculation) );
    }

    static pointer create( Examples::CalculatorInterface::pointer base )
    {
      return pointer( new Recalculator(base) );
    }
    
    ~Recalculator() { }

    virtual double add( double a, double b ) const {
      double result = a+b+2.0;
      m_signal_calculation.emit( "add", "+", result, a, b );
      return result;
    }

  protected:

    void on_calculation( const std::string& op, const std::string& opsym, double result, double a, double b )
    {
      if ( opsym.empty() ) return;
      switch (opsym[0])
      {
        case '+': this->add(a,b);      break;
        case '-': this->subtract(a,b); break;
        case '*': this->multiply(a,b); break;
        case '/': this->divide(a,b);   break;
      }
    }

};


int main()
{
  DBus::init();

  DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create();

  DBus::Connection::pointer connection = dispatcher->create_connection( DBus::BUS_SESSION );

  DBus::Example::CalculatorProxy::pointer proxy = DBus::Example::CalculatorProxy::create(connection);

  proxy->set_name( "calculator" );

  Watcher watcher;

  watcher.watch( proxy );

  Recalculator::pointer recalculator = Recalculator::create(proxy);

  recalculator->set_name( "re-calculator" );

  watcher.watch( recalculator );

  std::cout << "Running" << std::flush;
  
  for (int i=0; i < 30; i++)
  {
    std::cout << "." << std::flush;
    sleep(1);
  }
  
  std::cout << std::endl;

  return 0;
}

