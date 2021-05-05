/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2017
 *
 *  Last modified:
 *     $Date: 2017-02-10 12:39:13 +1100 (Fri, 10 Feb 2017) $ by $Author: schulte $
 *     $Revision: 15401 $
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

  /// Class for sharing data between spaces
  template<class Data>
  class SharedData : public SharedHandle {
  protected:
    /// The object storing the actual data
    class SDO : public SharedHandle::Object {
    public:
      /// Data to be shared
      const Data d;
      /// Initialization
      SDO(const Data& d0);
      /// Copy constructor
      SDO(const SDO& sdo);
      /// Create new non-shared copy
      virtual Object* copy(void) const;
      /// Destructor
      virtual ~SDO(void);
    };
  public:
    /// Initialize with data \a d
    SharedData(const Data& d);
    /// Create as uninitialized
    SharedData(void);
    /// Copy constructor
    SharedData(const SharedData& sd);
    /// Assignment operator
    SharedData& operator =(const SharedData& sd);
    /// Provide access to data
    const Data& operator ()(void) const;
    /// Update during cloning
    void update(Space& home, bool share, SharedHandle& sh);
    /// Destructors
    ~SharedData(void);
  };


  template<class Data>
  forceinline
  SharedData<Data>::SDO::SDO(const Data& d0)
    : d(d0) {}

  template<class Data>
  forceinline
  SharedData<Data>::SDO::SDO(const SDO& sdo)
    : d(sdo.d) {}

  template<class Data>
  forceinline SharedHandle::Object*
  SharedData<Data>::SDO::copy(void) const {
    return new SDO(*this);
  }

  template<class Data>
  forceinline
  SharedData<Data>::SDO::~SDO(void) {}


  template<class Data>
  forceinline
  SharedData<Data>::SharedData(const Data& d) 
    : SharedHandle(new SDO(d)) {}

  template<class Data>
  forceinline
  SharedData<Data>::SharedData(void) {}

  template<class Data>
  forceinline
  SharedData<Data>::SharedData(const SharedData<Data>& sd)
    : SharedHandle(sd) {}

  template<class Data>
  forceinline SharedData<Data>&
  SharedData<Data>::operator =(const SharedData<Data>& sd) {
    return static_cast<SharedData&>(SharedHandle::operator =(sd));
  }

  template<class Data>
  forceinline const Data&
  SharedData<Data>::operator ()(void) const {
    return static_cast<SDO*>(object())->d;
  }

  template<class Data>
  forceinline void
  SharedData<Data>::update(Space& home, bool share, SharedHandle& sh) {
    SharedHandle::update(home,share,sh);
  }

  template<class Data>
  forceinline
  SharedData<Data>::~SharedData(void) {}

}

// STATISTICS: kernel-other
