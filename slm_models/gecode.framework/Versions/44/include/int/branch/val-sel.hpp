/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2012
 *
 *  Last modified:
 *     $Date: 2017-03-11 02:52:37 +1100 (Sat, 11 Mar 2017) $ by $Author: schulte $
 *     $Revision: 15569 $
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

namespace Gecode { namespace Int { namespace Branch {

  template<class View>
  forceinline
  ValSelMin<View>::ValSelMin(Space& home, const ValBranch<Var>& vb)
    : ValSel<View,int>(home,vb) {}
  template<class View>
  forceinline
  ValSelMin<View>::ValSelMin(Space& home, bool shared, ValSelMin& vs)
    : ValSel<View,int>(home,shared,vs) {}
  template<class View>
  forceinline int
  ValSelMin<View>::val(const Space&, View x, int) {
    return x.min();
  }

  template<class View>
  forceinline
  ValSelMax<View>::ValSelMax(Space& home, const ValBranch<Var>& vb)
    : ValSel<View,int>(home,vb) {}
  template<class View>
  forceinline
  ValSelMax<View>::ValSelMax(Space& home, bool shared, ValSelMax& vs)
    : ValSel<View,int>(home,shared,vs) {}
  template<class View>
  forceinline int
  ValSelMax<View>::val(const Space&, View x, int) {
    return x.max();
  }

  template<class View>
  forceinline
  ValSelMed<View>::ValSelMed(Space& home, const ValBranch<Var>& vb)
    : ValSel<View,int>(home,vb) {}
  template<class View>
  forceinline
  ValSelMed<View>::ValSelMed(Space& home, bool shared, ValSelMed& vs)
    : ValSel<View,int>(home,shared,vs) {}
  template<class View>
  forceinline int
  ValSelMed<View>::val(const Space&, View x, int) {
    return x.med();
  }

  template<class View>
  forceinline
  ValSelAvg<View>::ValSelAvg(Space& home, const ValBranch<Var>& vb)
    : ValSel<View,int>(home,vb) {}
  template<class View>
  forceinline
  ValSelAvg<View>::ValSelAvg(Space& home, bool shared, ValSelAvg& vs)
    : ValSel<View,int>(home,shared,vs) {}
  template<class View>
  forceinline int
  ValSelAvg<View>::val(const Space&, View x, int) {
    return (x.width() == 2U) ? x.min() : ((x.min()+x.max()) / 2);
  }

  template<class View>
  forceinline
  ValSelRnd<View>::ValSelRnd(Space& home, const ValBranch<Var>& vb)
    : ValSel<View,int>(home,vb), r(vb.rnd()) {}
  template<class View>
  forceinline
  ValSelRnd<View>::ValSelRnd(Space& home, bool shared, ValSelRnd& vs)
    : ValSel<View,int>(home,shared,vs) {
    r.update(home,shared,vs.r);
  }
  template<class View>
  forceinline int
  ValSelRnd<View>::val(const Space&, View x, int) {
    unsigned int p = r(x.size());
    for (ViewRanges<View> i(x); i(); ++i) {
      if (i.width() > p)
        return i.min() + static_cast<int>(p);
      p -= i.width();
    }
    GECODE_NEVER;
    return 0;
  }
  template<class View>
  forceinline bool
  ValSelRnd<View>::notice(void) const {
    return true;
  }
  template<class View>
  forceinline void
  ValSelRnd<View>::dispose(Space&) {
    r.~Rnd();
  }

  forceinline
  ValSelRangeMin::ValSelRangeMin(Space& home, const ValBranch<IntVar>& vb)
    : ValSel<IntView,int>(home,vb) {}
  forceinline
  ValSelRangeMin::ValSelRangeMin(Space& home, bool shared, ValSelRangeMin& vs)
    : ValSel<IntView,int>(home,shared,vs) {}
  forceinline int
  ValSelRangeMin::val(const Space&, IntView x, int) {
    if (x.range()) {
      return (x.width() == 2) ? x.min() : (x.min() + (x.max()-x.min())/2);
    } else {
      ViewRanges<View> r(x);
      return r.max();
    }
  }

  forceinline
  ValSelRangeMax::ValSelRangeMax(Space& home, const ValBranch<IntVar>& vb)
    : ValSel<IntView,int>(home,vb) {}
  forceinline
  ValSelRangeMax::ValSelRangeMax(Space& home, bool shared, ValSelRangeMax& vs)
    : ValSel<IntView,int>(home,shared,vs) {}
  forceinline int
  ValSelRangeMax::val(const Space&, IntView x, int) {
    if (x.range()) {
      return (x.width() == 2) ? x.max() : (x.max() - (x.max()-x.min())/2);
    } else {
      int min;
      ViewRanges<IntView> r(x);
      do {
        min = r.min(); ++r;
      } while (r());
      return min;
    }
  }

}}}

// STATISTICS: int-branch

