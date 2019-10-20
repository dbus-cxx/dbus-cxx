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
   * HandlerResult::HANDLED.
   *
   * If no slot returned HandlerResult::HANDLED it will return HandlerResult::NEEDS_MEMORY if any
   * handler indicated this condition.
   *
   * Otherwise it will return HandlerResult::NOT_HANDLED.
   */
  struct MessageHandlerAccumulator {
    typedef HandlerResult result_type;

    template <typename T_iterator>
    result_type operator()( T_iterator first, T_iterator last ) const {
      HandlerResult retval = HandlerResult::NOT_HANDLED;
      while ( first != last ) {
        switch ( *first )
        {
          case HandlerResult::HANDLED:
            return HandlerResult::HANDLED;
          case HandlerResult::NEEDS_MEMORY:
            retval = HandlerResult::NEEDS_MEMORY;
            // no break because we'll slide through to the next case
          case HandlerResult::NOT_HANDLED:
            ++first;
        }
      }
      return retval;
    }
  };

  /**
   * This accumulator will try each slot and stop when the first slot returns
   * FilterResult::FILTER.
   *
   * If no slot returned FilterResult::FILTER it will return FilterResult::NEEDS_MEMORY if any
   * handler indicated this condition.
   *
   * Otherwise it will return FilterResult::DONT_FILTER.
   */
  struct FilterAccumulator {
    typedef FilterResult result_type;

    template <typename T_iterator>
        result_type operator()( T_iterator first, T_iterator last ) const {
      FilterResult retval = FilterResult::DONT_FILTER;
      while ( first != last ) {
        switch ( *first )
        {
          case FilterResult::FILTER:
            return FilterResult::FILTER;
          case FilterResult::NEEDS_MEMORY:
            retval = FilterResult::NEEDS_MEMORY;
            // no break because we'll slide through to the next case
          case FilterResult::DONT_FILTER:
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
