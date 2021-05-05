/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2009
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

#include <cmath>

namespace Gecode {

  /// Global propagator information
  class GPI {
  public:
    /// Class for storing timed-decay value
    class Info {
    public:
      /// Propagator identifier
      unsigned int pid;
      /// Group identifier
      unsigned int gid;
      /// The afc value
      double afc;
      /// Initialize
      void init(unsigned int pid, unsigned int gid);
    };
  private:
    /// Block of propagator information
    class Block : public HeapAllocated {
    public:
      /// Number of propagator information entries in a block
      static const int n_info = 8192;
      /// Info entries
      Info info[n_info];
      /// Next block
      Block* next;
      /// Number of free blocks
      int free;
      /// Initialize
      Block(void);
      /// Rescale used afc values in entries
      void rescale(void);
    };
    /// The object containing the shared information
    class Object : public HeapAllocated {
    private:
      /// The first block
      Block fst;
      /// The current block
      Block* b;
      /// Mutex to synchronize globally shared access
      Support::Mutex m;
      /// The inverse decay factor
      double invd;
      /// Next free propagator id
      unsigned int pid;
      /// How many spaces use this object
      unsigned int use_cnt;
    public:
      /// Initialize
      Object(void);
      /// Increment use counter
      void inc(void);
      /// Set decay factor to \a d
      void decay(double d);
      /// Return decay factor
      double decay(void) const;
      /// Increment failure count
      void fail(Info& c);
      /// Allocate new actor info
      Info* allocate(unsigned int gid);
      /// Dispose if unused
      void dispose(void);
    };
    /// Pointer to shared object
    Object* o;
  public:
    /// Initialize
    GPI(void);
    /// Copy during cloning
    GPI(const GPI& ga);
    /// Destructor
    ~GPI(void);
    /// Set decay factor to \a d
    void decay(double d);
    /// Return decay factor
    double decay(void) const;
    /// Increment failure count
    void fail(Info& c);
    /// Allocate new actor info
    Info* allocate(unsigned int gid);
  };


  forceinline void
  GPI::Info::init(unsigned int pid0, unsigned int gid0) {
    pid=pid0; gid=gid0; afc=1.0;
  }


  forceinline
  GPI::Block::Block(void)
    : next(NULL), free(n_info) {}

  forceinline void
  GPI::Block::rescale(void) {
    for (int i=free; i < n_info; i++)
      info[i].afc *= Kernel::Config::rescale;
  }


  forceinline
  GPI::Object::Object(void)
    : b(&fst), invd(1.0), pid(0U), use_cnt(1U) {}

  forceinline void
  GPI::Object::inc(void) {
    m.acquire();
    use_cnt++;
    m.release();
  }

  forceinline void
  GPI::Object::fail(Info& c) {
    m.acquire();
    c.afc = invd * (c.afc + 1.0);
    if (c.afc > Kernel::Config::rescale_limit)
      for (Block* i = b; i != NULL; i = i->next)
        i->rescale();
    m.release();
  }

  forceinline double
  GPI::Object::decay(void) const {
    double d;
    const_cast<GPI::Object&>(*this).m.acquire();
    d = 1.0 / invd;
    const_cast<GPI::Object&>(*this).m.release();
    return d;
  }

  forceinline void
  GPI::Object::decay(double d) {
    m.acquire();
    invd = 1.0 / d;
    m.release();
  }

  forceinline GPI::Info*
  GPI::Object::allocate(unsigned int gid) {
    Info* c;
    m.acquire();
    if (b->free == 0) {
      Block* n = new Block;
      n->next = b; b = n;
    }
    c = &b->info[--b->free];
    m.release();
    c->init(pid++,gid);
    return c;
  }

  forceinline void
  GPI::Object::dispose(void) {
    m.acquire();
    if (--use_cnt > 0) {
      m.release();
      return;
    }
    // We are on our own here!
    m.release();
    Block* n = b;
    while (n != &fst) {
      Block* d = n;
      n = n->next;
      delete d;
    }
    delete this;
  }



  forceinline
  GPI::GPI(void) : o(new Object) {}

  forceinline
  GPI::GPI(const GPI& gpi) : o(gpi.o) {
    o->inc();
  }

  forceinline
  GPI::~GPI(void) {
    o->dispose();
  }

  forceinline void
  GPI::fail(Info& c) {
    o->fail(c);
  }

  forceinline double
  GPI::decay(void) const {
    return o->decay();
  }

  forceinline void
  GPI::decay(double d) {
    o->decay(d);
  }

  forceinline GPI::Info*
  GPI::allocate(unsigned int gid) {
    return o->allocate(gid);
  }

}

// STATISTICS: kernel-prop
