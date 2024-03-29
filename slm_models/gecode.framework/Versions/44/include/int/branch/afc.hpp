/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2013
 *
 *  Last modified:
 *     $Date: 2017-02-16 22:11:51 +1100 (Thu, 16 Feb 2017) $ by $Author: schulte $
 *     $Revision: 15434 $
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

  forceinline
  IntAFC::IntAFC(void) {}

  forceinline
  IntAFC::IntAFC(const IntAFC& a)
    : AFC(a) {}

  forceinline IntAFC&
  IntAFC::operator =(const IntAFC& a) {
    return static_cast<IntAFC&>(AFC::operator =(a));
  }

  forceinline
  IntAFC::IntAFC(Home home, const IntVarArgs& x, double d) {
    AFC::init(home,x,d);
  }

  forceinline void
  IntAFC::init(Home home, const IntVarArgs& x, double d) {
    AFC::init(home,x,d);
  }



  forceinline
  BoolAFC::BoolAFC(void) {}

  forceinline
  BoolAFC::BoolAFC(const BoolAFC& a)
    : AFC(a) {}

  forceinline BoolAFC&
  BoolAFC::operator =(const BoolAFC& a) {
    return static_cast<BoolAFC&>(AFC::operator =(a));
  }

  forceinline
  BoolAFC::BoolAFC(Home home, const BoolVarArgs& x, double d) {
    AFC::init(home,x,d);
  }

  forceinline void
  BoolAFC::init(Home home, const BoolVarArgs& x, double d) {
    AFC::init(home,x,d);
  }

}

// STATISTICS: int-branch
