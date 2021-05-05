/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Contributing authors:
 *     Gabor Szokoli <szokoli@gecode.org>
 *     Guido Tack <tack@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2012
 *     Gabor Szokoli, 2004
 *     Guido Tack, 2004
 *
 *  Last modified:
 *     $Date: 2017-04-02 04:27:10 +1000 (Sun, 02 Apr 2017) $ by $Author: schulte $
 *     $Revision: 15623 $
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

namespace Gecode { namespace Set { namespace Branch {

  // Minimum merit
  forceinline
  MeritMin::MeritMin(Space& home, const VarBranch<Var>& vb)
    : MeritBase<SetView,int>(home,vb) {}
  forceinline
  MeritMin::MeritMin(Space& home, bool shared, MeritMin& m)
    : MeritBase<SetView,int>(home,shared,m) {}
  forceinline int
  MeritMin::operator ()(const Space&, SetView x, int) {
    UnknownRanges<SetView> u(x);
    return u.min();
  }

  // Maximum merit
  forceinline
  MeritMax::MeritMax(Space& home, const VarBranch<Var>& vb)
    : MeritBase<SetView,int>(home,vb) {}
  forceinline
  MeritMax::MeritMax(Space& home, bool shared, MeritMax& m)
    : MeritBase<SetView,int>(home,shared,m) {}
  forceinline int
  MeritMax::operator ()(const Space&, SetView x, int) {
    int max = Limits::max;
    for (UnknownRanges<SetView> u(x); u(); ++u)
      max = u.max();
    return max;
  }

  // Size merit
  forceinline
  MeritSize::MeritSize(Space& home, const VarBranch<Var>& vb)
    : MeritBase<SetView,unsigned int>(home,vb) {}
  forceinline
  MeritSize::MeritSize(Space& home, bool shared, MeritSize& m)
    : MeritBase<SetView,unsigned int>(home,shared,m) {}
  forceinline unsigned int
  MeritSize::operator ()(const Space&, SetView x, int) {
    return x.unknownSize();
  }

  // Degree over size merit
  forceinline
  MeritDegreeSize::MeritDegreeSize(Space& home, const VarBranch<Var>& vb)
    : MeritBase<SetView,double>(home,vb) {}
  forceinline
  MeritDegreeSize::MeritDegreeSize(Space& home, bool shared,
                                   MeritDegreeSize& m)
    : MeritBase<SetView,double>(home,shared,m) {}
  forceinline double
  MeritDegreeSize::operator ()(const Space&, SetView x, int) {
    return static_cast<double>(x.unknownSize()) /
      static_cast<double>(x.degree());
  }

  // AFC over size merit
  forceinline
  MeritAFCSize::MeritAFCSize(Space& home, const VarBranch<Var>& vb)
    : MeritBase<SetView,double>(home,vb), afc(vb.afc()) {}
  forceinline
  MeritAFCSize::MeritAFCSize(Space& home, bool shared, MeritAFCSize& m)
    : MeritBase<SetView,double>(home,shared,m) {
    afc.update(home,shared,m.afc);
  }
  forceinline double
  MeritAFCSize::operator ()(const Space&, SetView x, int) {
    return x.afc() / static_cast<double>(x.unknownSize());
  }
  forceinline bool
  MeritAFCSize::notice(void) const {
    return true;
  }
  forceinline void
  MeritAFCSize::dispose(Space&) {
    afc.~AFC();
  }

  // Action over size merit
  forceinline
  MeritActionSize::MeritActionSize(Space& home,
                                   const VarBranch<Var>& vb)
    : MeritBase<SetView,double>(home,vb), action(vb.action()) {}
  forceinline
  MeritActionSize::MeritActionSize(Space& home, bool shared,
                                   MeritActionSize& m)
    : MeritBase<SetView,double>(home,shared,m) {
    action.update(home, shared, m.action);
  }
  forceinline double
  MeritActionSize::operator ()(const Space&, SetView x, int i) {
    return action[i] / static_cast<double>(x.unknownSize());
  }
  forceinline bool
  MeritActionSize::notice(void) const {
    return true;
  }
  forceinline void
  MeritActionSize::dispose(Space&) {
    action.~Action();
  }


  // CHB Q-score over size merit
  forceinline
  MeritCHBSize::MeritCHBSize(Space& home,
                             const VarBranch<Var>& vb)
    : MeritBase<SetView,double>(home,vb), chb(vb.chb()) {}
  forceinline
  MeritCHBSize::MeritCHBSize(Space& home, bool shared,
                             MeritCHBSize& m)
    : MeritBase<SetView,double>(home,shared,m) {
    chb.update(home, shared, m.chb);
  }
  forceinline double
  MeritCHBSize::operator ()(const Space&, SetView x, int i) {
    return chb[i] / static_cast<double>(x.unknownSize());
  }
  forceinline bool
  MeritCHBSize::notice(void) const {
    return true;
  }
  forceinline void
  MeritCHBSize::dispose(Space&) {
    chb.~CHB();
  }

}}}

// STATISTICS: set-branch
