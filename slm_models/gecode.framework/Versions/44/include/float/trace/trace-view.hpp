/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2016
 *
 *  Last modified:
 *     $Date: 2017-03-18 09:04:57 +1100 (Sat, 18 Mar 2017) $ by $Author: schulte $
 *     $Revision: 15597 $
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

namespace Gecode { namespace Float {

  /// Float trace view
  class FloatTraceView {
  public:
    /// Default constructor (initializes with no view)
    FloatTraceView(void);
    /// Duplicate view \a y
    FloatTraceView(Space& home, FloatView y);
    /// Update duplicated view from view \a y and delta \a d
    void prune(Space& home, FloatView y, const Delta& d);
    /// Update during cloning
    void update(Space& home, bool share, FloatTraceView x);
    /// Return slack of \a x
    static FloatNum slack(FloatView x);
  };

  forceinline
  FloatTraceView::FloatTraceView(void) {}
  forceinline
  FloatTraceView::FloatTraceView(Space&, FloatView) {}
  forceinline void
  FloatTraceView::prune(Space&, FloatView, const Delta&) {}
  forceinline void
  FloatTraceView::update(Space&, bool, FloatTraceView) {}
  forceinline FloatNum
  FloatTraceView::slack(FloatView x) {
    return x.size();
  }

}}

// STATISTICS: float-trace
