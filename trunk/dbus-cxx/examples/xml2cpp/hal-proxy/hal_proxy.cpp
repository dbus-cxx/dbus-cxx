/***************************************************************************
 *   Copyright (C) 2007,2008,2009 by Rick L. Vinyard, Jr.                  *
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
#include <hal_manager_proxy.h>
#include <iostream>

DBus::Hal::ManagerProxy::pointer manager;

void on_device_added(std::string device);
void on_device_removed(std::string device);

int main()
{
  DBus::init();

  DBus::Dispatcher dispatcher;

  DBus::Connection::pointer connection = dispatcher.create_connection( DBus::BUS_SYSTEM );

  manager = DBus::Hal::ManagerProxy::create(connection);
  manager->signal_DeviceAdded().connect( sigc::ptr_fun(on_device_added) );
  manager->signal_DeviceRemoved().connect( sigc::ptr_fun(on_device_removed) );

  dispatcher.start();

  std::cout << "Running" << std::flush;
  
  for (int i=0; i < 30; i++)
  {
    std::cout << "." << std::flush;
    sleep(1);
  }
  
  dispatcher.stop();

  std::cout << std::endl;

  return 0;
}

void exists(const std::string& device)
{
  bool exist = manager->DeviceExists( device );
  std::cout << "!!!!! Device " << device << (exist?" exists":" doesn't exist") << std::endl;
}

void on_device_added(std::string device)
{
  std::cout << "+++++ Device Added: " << device << std::endl;
  exists(device);
}

void on_device_removed(std::string device)
{
  std::cout << "----- Device Removed: " << device << std::endl;
  exists(device);
}
