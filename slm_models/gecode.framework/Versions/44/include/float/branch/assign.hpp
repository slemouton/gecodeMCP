/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2012
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
  FloatAssign::FloatAssign(Select s0)
    : s(s0) {}

  forceinline
  FloatAssign::FloatAssign(Rnd r)
    : ValBranch<FloatVar>(r), s(SEL_RND) {}

  forceinline
  FloatAssign::FloatAssign(FloatBranchVal v, FloatBranchCommit c)
    : ValBranch<FloatVar>(v,c), s(SEL_VAL_COMMIT) {}

  forceinline FloatAssign::Select
  FloatAssign::select(void) const {
    return s;
  }


  inline FloatAssign
  FLOAT_ASSIGN_MIN(void) {
    return FloatAssign(FloatAssign::SEL_MIN);
  }

  inline FloatAssign
  FLOAT_ASSIGN_MAX(void) {
    return FloatAssign(FloatAssign::SEL_MAX);
  }

  inline FloatAssign
  FLOAT_ASSIGN_RND(Rnd r) {
    return FloatAssign(r);
  }

  inline FloatAssign
  FLOAT_ASSIGN(FloatBranchVal v, FloatBranchCommit c) {
    return FloatAssign(v,c);
  }

}

// STATISTICS: float-branch
