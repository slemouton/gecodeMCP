/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2012
 *
 *  Last modified:
 *     $Date: 2017-03-08 21:58:56 +1100 (Wed, 08 Mar 2017) $ by $Author: schulte $
 *     $Revision: 15562 $
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
  FloatVarBranch::FloatVarBranch(void)
    : s(SEL_NONE) {}

  forceinline
  FloatVarBranch::FloatVarBranch(Select s0, BranchTbl t)
    : VarBranch<FloatVar>(t), s(s0) {}

  forceinline
  FloatVarBranch::FloatVarBranch(Rnd r)
    : VarBranch<FloatVar>(r), s(SEL_RND) {}

  forceinline
  FloatVarBranch::FloatVarBranch(Select s0, double d, BranchTbl t)
    : VarBranch<FloatVar>(d,t), s(s0) {}

  forceinline
  FloatVarBranch::FloatVarBranch(Select s0, FloatAFC a, BranchTbl t)
    : VarBranch<FloatVar>(a,t), s(s0) {}

  forceinline
  FloatVarBranch::FloatVarBranch(Select s0, FloatAction a, BranchTbl t)
    : VarBranch<FloatVar>(a,t), s(s0) {}

  forceinline
  FloatVarBranch::FloatVarBranch(Select s0, FloatCHB c, BranchTbl t)
    : VarBranch<FloatVar>(c,t), s(s0) {}

  forceinline
  FloatVarBranch::FloatVarBranch(Select s0, FloatBranchMerit mf, BranchTbl t)
    : VarBranch<FloatVar>(mf,t), s(s0) {}

  forceinline FloatVarBranch::Select
  FloatVarBranch::select(void) const {
    return s;
  }

  forceinline void
  FloatVarBranch::expand(Home home, const FloatVarArgs& x) {
    switch (select()) {
    case SEL_AFC_MIN: case SEL_AFC_MAX:
    case SEL_AFC_SIZE_MIN: case SEL_AFC_SIZE_MAX:
      if (!_afc)
        _afc = FloatAFC(home,x,decay());
      break;
    case SEL_ACTION_MIN: case SEL_ACTION_MAX:
    case SEL_ACTION_SIZE_MIN: case SEL_ACTION_SIZE_MAX:
      if (!_act)
        _act = FloatAction(home,x,decay());
      break;
    case SEL_CHB_MIN: case SEL_CHB_MAX:
    case SEL_CHB_SIZE_MIN: case SEL_CHB_SIZE_MAX:
      if (!_chb)
        _chb = FloatCHB(home,x);
      break;
    default: ;
    }
  }


  inline FloatVarBranch
  FLOAT_VAR_NONE(void) {
    return FloatVarBranch(FloatVarBranch::SEL_NONE,nullptr);
  }

  inline FloatVarBranch
  FLOAT_VAR_MERIT_MIN(FloatBranchMerit bm, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_MERIT_MIN,bm,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_MERIT_MAX(FloatBranchMerit bm, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_MERIT_MAX,bm,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_RND(Rnd r) {
    return FloatVarBranch(r);
  }

  inline FloatVarBranch
  FLOAT_VAR_DEGREE_MIN(BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_DEGREE_MIN,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_DEGREE_MAX(BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_DEGREE_MAX,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_AFC_MIN(double d, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_AFC_MIN,d,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_AFC_MIN(FloatAFC a, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_AFC_MIN,a,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_AFC_MAX(double d, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_AFC_MAX,d,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_AFC_MAX(FloatAFC a, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_AFC_MAX,a,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_ACTION_MIN(double d, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_ACTION_MIN,d,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_ACTION_MIN(FloatAction a, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_ACTION_MIN,a,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_ACTION_MAX(double d, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_ACTION_MAX,d,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_ACTION_MAX(FloatAction a, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_ACTION_MAX,a,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_CHB_MIN(BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_CHB_MIN,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_CHB_MIN(FloatCHB c, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_CHB_MIN,c,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_CHB_MAX(BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_CHB_MAX,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_CHB_MAX(FloatCHB c, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_CHB_MAX,c,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_MIN_MIN(BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_MIN_MIN,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_MIN_MAX(BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_MIN_MAX,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_MAX_MIN(BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_MAX_MIN,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_MAX_MAX(BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_MAX_MAX,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_SIZE_MIN(BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_SIZE_MIN,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_SIZE_MAX(BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_SIZE_MAX,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_DEGREE_SIZE_MIN(BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_DEGREE_SIZE_MIN,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_DEGREE_SIZE_MAX(BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_DEGREE_SIZE_MAX,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_AFC_SIZE_MIN(double d, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_AFC_SIZE_MIN,d,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_AFC_SIZE_MIN(FloatAFC a, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_AFC_SIZE_MIN,a,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_AFC_SIZE_MAX(double d, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_AFC_SIZE_MAX,d,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_AFC_SIZE_MAX(FloatAFC a, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_AFC_SIZE_MAX,a,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_ACTION_SIZE_MIN(double d, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_ACTION_SIZE_MIN,d,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_ACTION_SIZE_MIN(FloatAction a, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_ACTION_SIZE_MIN,a,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_ACTION_SIZE_MAX(double d, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_ACTION_SIZE_MAX,d,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_ACTION_SIZE_MAX(FloatAction a, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_ACTION_SIZE_MAX,a,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_CHB_SIZE_MIN(BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_CHB_SIZE_MIN,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_CHB_SIZE_MIN(FloatCHB c, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_CHB_SIZE_MIN,c,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_CHB_SIZE_MAX(BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_CHB_SIZE_MAX,tbl);
  }

  inline FloatVarBranch
  FLOAT_VAR_CHB_SIZE_MAX(FloatCHB c, BranchTbl tbl) {
    return FloatVarBranch(FloatVarBranch::SEL_CHB_SIZE_MAX,c,tbl);
  }

}

// STATISTICS: float-branch
