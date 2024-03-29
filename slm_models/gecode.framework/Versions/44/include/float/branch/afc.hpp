/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2013
 *
 *  Last modified:
 *     $Date: 2013-02-19 07:53:58 +1100 (Tue, 19 Feb 2013) $ by $Author: schulte $
 *     $Revision: 13310 $
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
  FloatAFC::FloatAFC(void) {}

  forceinline
  FloatAFC::FloatAFC(const FloatAFC& a)
    : AFC(a) {}

  forceinline FloatAFC&
  FloatAFC::operator =(const FloatAFC& a) {
    return static_cast<FloatAFC&>(AFC::operator =(a));
  }

  forceinline
  FloatAFC::FloatAFC(Home home, const FloatVarArgs& x, double d) {
    AFC::init(home,x,d);
  }

  forceinline void
  FloatAFC::init(Home home, const FloatVarArgs& x, double d) {
    AFC::init(home,x,d);
  }

}

// STATISTICS: float-branch
