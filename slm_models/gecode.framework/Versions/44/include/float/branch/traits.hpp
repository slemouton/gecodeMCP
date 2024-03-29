/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
*
 *  Copyright:
 *     Christian Schulte, 2012
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

namespace Gecode {

  /// Traits of %FloatVar for branching
  template<>
  class BranchTraits<FloatVar> {
  public:
    /// Type for the branching filter function
    typedef FloatBranchFilter Filter;
    /// Type for the branching merit function
    typedef FloatBranchMerit Merit;
    /// Type for the branching value function
    typedef FloatBranchVal Val;
    /// Return type of the branching value function
    typedef FloatNumBranch ValType;
    /// Type for the branching commit function
    typedef FloatBranchCommit Commit;
  };

}

// STATISTICS: float-branch
