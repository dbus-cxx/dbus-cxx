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
#include <iostream>

#include <cppunit/TextTestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TextOutputter.h>

#include "messageiteratortests.h"
#include "callmessagetests.h"
#include "pathclasstests.h"

CPPUNIT_TEST_SUITE_REGISTRATION( MessageIteratorTests );
CPPUNIT_TEST_SUITE_REGISTRATION( CallMessageTests );
CPPUNIT_TEST_SUITE_REGISTRATION( PathClassTests );

int main() {
  // Create the event manager and test controller
  CppUnit::TestResult controller;

  // Add a listener that collects test result
  CppUnit::TestResultCollector result;
  controller.addListener( &result );

  // Add a listener that print dots as test run.
  CppUnit::BriefTestProgressListener progress;
  controller.addListener( &progress );

  // Add the top suite to the test runner
  CppUnit::TestRunner runner;
  runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );
  runner.run( controller );

  CppUnit::TextOutputter output(&result, std::cout);
  std::cout << std::endl
      << "==================================================" << std::endl
      << "dbus-cxx Library Unit Tests: " << std::endl;
  output.printHeader();
  output.printStatistics();
  output.printFailures();

  return result.wasSuccessful() ? 0 : 1;
}

