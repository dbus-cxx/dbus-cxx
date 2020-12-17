/***************************************************************************
 *   Copyright (C) 2007,2008,2009,2010 by Rick L. Vinyard, Jr.             *
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
#include <dbus-cxx.h>
#include <iostream>
#include <unistd.h>

class Calculator: public DBus::ObjectProxy
{
  protected:

    Calculator(std::shared_ptr<DBus::Connection> conn):
      DBus::ObjectProxy(conn, "dbuscxx.example.calculator.server", "/dbuscxx/example/Calculator")
      {
        m_method_add = this->create_method<double(double,double)>("Calculator.Basic", "add");
        m_method_sub = this->create_method<double(double,double)>("Calculator.Basic", "sub");
        m_method_mul = this->create_method<double(double,double)>("Calculator.Basic", "mul");
        m_method_div = this->create_method<double(double,double)>("Calculator.Basic", "div");
        m_signal_calculation = this->create_signal<void(std::string,double,double,double)>("Calculator.Basic", "calculation");
      }

  public:

    typedef std::shared_ptr<Calculator> pointer;

    static pointer create(std::shared_ptr<DBus::Connection> conn)
    {
      return pointer(new Calculator(conn));
    }
    
    double add(double param1, double param2) { return (*m_method_add)(param1,param2); }

    double sub(double param1, double param2) { return (*m_method_sub)(param1,param2); }

    double mul(double param1, double param2) { return (*m_method_mul)(param1,param2); }

    double div(double param1, double param2) { return (*m_method_div)(param1,param2); }

    DBus::SignalProxy<void(std::string,double,double,double)>& signal_calculation() { return *m_signal_calculation; }

  protected:
    
    std::shared_ptr<DBus::MethodProxy<double(double,double)>> m_method_add;
    std::shared_ptr<DBus::MethodProxy<double(double,double)>> m_method_sub;
    std::shared_ptr<DBus::MethodProxy<double(double,double)>> m_method_mul;
    std::shared_ptr<DBus::MethodProxy<double(double,double)>> m_method_div;
    std::shared_ptr<DBus::SignalProxy<void(std::string,double,double,double)>> m_signal_calculation;
};


void print(std::string op, double param1, double param2, double result);

int main()
{
  std::shared_ptr<DBus::Dispatcher> dispatcher = DBus::StandaloneDispatcher::create();

  std::shared_ptr<DBus::Connection> connection = dispatcher->create_connection( DBus::BusType::SESSION );

  Calculator::pointer calculator = Calculator::create(connection);

  calculator->signal_calculation().connect( sigc::ptr_fun(print) );

  std::cout << "Running" << std::flush;
  
  for (int i=0; i < 10; i++)
  {
    std::cout << "." << std::flush;
    sleep(1);
  }
  
  std::cout << std::endl;

  return 0;
}

void print(std::string op, double param1, double param2, double result)
{
  std::cout << param1 << " " << op << " " << param2 << " = " << result << std::endl;
}
