/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2015
 *
 *  Last modified:
 *     $Date: 2016-04-20 01:19:45 +1000 (Wed, 20 Apr 2016) $ by $Author: schulte $
 *     $Revision: 14967 $
 *
 *  This file is part of Gecode, the generic constraint
 *  development environment:
 *     http://www.gecode.org
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

namespace Gecode {

  forceinline IntPropLevel
  vbd(IntPropLevel ipl) {
    return static_cast<IntPropLevel>(ipl &
                                     (IPL_DEF | IPL_VAL | IPL_BND | IPL_DOM));
  }

  forceinline IntPropLevel
  sm(IntPropLevel ipl) {
    return static_cast<IntPropLevel>(ipl &
                                     (IPL_SPEED | IPL_MEMORY));
  }

  forceinline IntPropLevel
  ba(IntPropLevel ipl) {
    return static_cast<IntPropLevel>(ipl & IPL_BASIC_ADVANCED);
  }

}

// STATISTICS: int-post

