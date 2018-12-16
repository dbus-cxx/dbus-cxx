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
#include <dbus-cxx/enums.h>

#ifndef DBUSCXX_ACCUMULATORS_H
#define DBUSCXX_ACCUMULATORS_H

namespace DBus
{
  /**
   * @defgroup accumulators Accumulators
   *
   * These are sigc::signal accumulators that are common to dbus-cxx.
   */
  //@{

  /**
   * This accumulator starts with a presumption of true and
   * checks to see if any result returned false
   */
  struct PredicateAccumulatorDefaultTrue {
    typedef bool result_type;

    template <typename T_iterator>
    result_type operator()( T_iterator first, T_iterator last ) const {
      bool result = true;
      while ( first != last ) {
        result = result and( *first );
      }
      return result;
    }
  };

  /**
   * This accumulator will interrupt on the first slot that returns true.
   * Otherwise it will return false if all slots are false.
   */
  struct InterruptablePredicateAccumulatorDefaultFalse {
    typedef bool result_type;

    template <typename T_iterator>
    result_type operator()( T_iterator first, T_iterator last ) const {
      while ( first != last ) {
        if ( *first ) return true;
        ++first;
      }
      return false;
    }
  };

  /**
   * This accumulator will try each slot and stop when the first slot returns
   * HANDLED.
   *
   * If no slot returned HANDLED it will return HANDLER_NEEDS_MEMORY if any
   * handler indicated this condition.
   *
   * Otherwise it will return NOT_HANDLED.
   */
  struct MessageHandlerAccumulator {
    typedef HandlerResult result_type;

    template <typename T_iterator>
    result_type operator()( T_iterator first, T_iterator last ) const {
      HandlerResult retval = NOT_HANDLED;
      while ( first != last ) {
        switch ( *first )
        {
          case HANDLED:
            return HANDLED;
          case HANDLER_NEEDS_MEMORY:
            retval = HANDLER_NEEDS_MEMORY;
            // no break because we'll slide through to the next case
          case NOT_HANDLED:
            ++first;
        }
      }
      return retval;
    }
  };

  /**
   * This accumulator will try each slot and stop when the first slot returns
   * FILTER.
   *
   * If no slot returned FILTER it will return FILTER_NEEDS_MEMORY if any
   * handler indicated this condition.
   *
   * Otherwise it will return DONT_FILTER.
   */
  struct FilterAccumulator {
    typedef FilterResult result_type;

    template <typename T_iterator>
        result_type operator()( T_iterator first, T_iterator last ) const {
      FilterResult retval = DONT_FILTER;
      while ( first != last ) {
        switch ( *first )
        {
          case FILTER:
            return FILTER;
          case FILTER_NEEDS_MEMORY:
            retval = FILTER_NEEDS_MEMORY;
            // no break because we'll slide through to the next case
          case DONT_FILTER:
            ++first;
        }
      }
      return retval;
    }
  };

  /**
   * This accumulator starts with a presumption of false and
   * checks to see if any result returned true
   */
  struct PredicateAccumulatorDefaultFalse {
    typedef bool result_type;

    template <typename T_iterator>
    result_type operator()( T_iterator first, T_iterator last ) const {
      bool result = false;
      while ( first != last ) {
        result = result or( *first );
        ++first;
      }
      return result;
    }
  };

  //@}

}

#endif
