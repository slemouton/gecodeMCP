/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Guido Tack <tack@gecode.org>
 *
 *  Copyright:
 *     Guido Tack, 2004
 *
 *  Last modified:
 *     $Date: 2009-05-14 02:59:23 +0200 (Do, 14 Mai 2009) $ by $Author: tack $
 *     $Revision: 9099 $
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

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/set.hh>

using namespace Gecode;

/**
 * \brief %Options for %Hamming problems
 *
 */

#define MAXSTRINGLENGTH 200000
static char result[MAXSTRINGLENGTH];


class HammingOptions : public Options {
private:
  /// Number of bits
  Driver::UnsignedIntOption _bits;
  /// Minimum distance
  Driver::UnsignedIntOption _distance;
  /// Number of symbols
  Driver::UnsignedIntOption _size;
  
public:
  /// Initialize options for example with name \a s
  HammingOptions(const char* s, unsigned int bits0,
                 unsigned int distance0, unsigned int size0)
  : Options(s)
  , _bits("-bits","word size in bits",bits0)
  , _distance("-distance","minimum distance",distance0)
  , _size("-size","number of symbols",size0) {
    add(_bits);
    add(_distance);
    add(_size);
  }
  
  /// Return number of bits
  unsigned int bits(void) const { return _bits.value(); }
  /// Return minimum distance
  unsigned int distance(void) const { return _distance.value(); }
  /// Return number of symbols
  unsigned int size(void) const { return _size.value(); }
  
};

/**
 * \brief %Example: Generating %Hamming codes
 *
 * Generate a Hamming code that fits in b-bit words to code n symbols where
 * the Hamming distance between every two symbol codes is at least d.
 * The Hamming distance between two words is the number of bit positions
 * where they differ.
 *
 * \ingroup ExProblem
 *
 */
class Hamming : public Script {
private:
  /// The hamming code
  SetVarArray xs;
public:
  /// Actual model
  Hamming(const HammingOptions& opt) :
    Script(opt),
    xs(*this,opt.size(),IntSet::empty,1,opt.bits()) {
    SetVarArray cxs(*this,xs.size());
    for (int i=0; i<xs.size(); i++)
      rel(*this, xs[i], SRT_CMPL, cxs[i]);

    for (int i=0; i<xs.size(); i++) {
      SetVar y = xs[i];
      SetVar cy = cxs[i];
      for (int j=i+1; j<xs.size(); j++) {
        SetVar x = xs[j];
        SetVar cx = cxs[j];

        SetVar xIntCy(*this);
        SetVar yIntCx(*this);

        rel(*this, x, SOT_INTER, cy, SRT_EQ, xIntCy);
        rel(*this, y, SOT_INTER, cx, SRT_EQ, yIntCx);
        IntVar xIntCyCard(*this,0,x.cardMax());
        IntVar yIntCxCard(*this,0,y.cardMax());
        cardinality(*this, xIntCy, xIntCyCard);
        cardinality(*this, yIntCx, yIntCxCard);
        rel(*this, xIntCyCard+yIntCxCard >= opt.distance());
      }
    }

    branch(*this, xs, SET_VAR_NONE(), SET_VAL_MIN_INC());
//		branch(*this, xs, SET_VAR_RND, SET_VAL_MAX_INC);

  }

  /// Print solution
  virtual void
  print(std::ostream& os) const {
	  for (int i=0; i<xs.size(); i++) {
		   os << "(" ;
		  for (int j=0;j<=xs[i].glbMax();j++)
			  if (xs[i].contains(j))
				  os << j << " " ;
		    os << ")" ;
		  os << "\t[" << i << "] = " << xs[i] << std::endl;
		  
		  
	  }
	  print_to_string();
  }
	
	virtual void
	print_to_string() const {	
		std::ostringstream outputstring;
		outputstring << "(" ;
		
		for (int i=0; i<xs.size(); i++) {
			outputstring << "(" ;
			for (int j=0;j<=xs[i].glbMax();j++)
				if (xs[i].contains(j))
					outputstring << j << " " ;
		    outputstring << ")" ;
		}
		outputstring << ")" ;

		strcpy(result,outputstring.str().c_str());		
		
	}

  /// Constructor for copying \a s
  Hamming(Hamming& s) : Script(s) {
    xs.update(*this, s.xs);
  }
  /// Copy during cloning
  virtual Space*
  copy(void) {
    return new Hamming(*this);
  }

};

/** \brief Main-function
 *  \relates Hamming
 */
#if SCRIPT
int main(int argc, char* argv[]){
#else
int main_hamming (int argc, char* argv[]){
#endif

  HammingOptions opt("Hamming",20,3,32);
  opt.parse(argc,argv);
  Script::run<Hamming,DFS,HammingOptions>(opt);
  return 0;
}


/* function to be called in the framework version */


extern "C" {
	char *hammingG(int bits,int distance,int size,int solutions,int timelimit,char *resultat)
	{ 
		
		HammingOptions opt("Hamming",bits,distance,size);
		
		opt.iterations(5);
		opt.solutions(solutions);
		
		opt.time(timelimit);		
				
		strcpy(result,"nilnil");
		
		Script::run<Hamming,DFS,HammingOptions>(opt);
		
		sprintf(resultat, "%s", result);
		
		return result;
	}
	
};
