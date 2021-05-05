/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Serge Lemouton
 */

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

#include <cstdlib>
#include <fstream>

//#include "all-interval-omgg.h"


using namespace Gecode;

/**
 * \brief %Example: All-interval series
 *
 * An all-interval series of length \f$n\f$ is a sequence
 * \f[
 * (x_0,x_1,\ldots,n_{n-1})
 * \f]
 * where each \f$x_i\f$ is an integer between \f$0\f$ and \f$n-1\f$
 * such that the following conditions hold:
 *  - the \f$x_i\f$ are a permutation of \f$\{0,1,\ldots,n-1\}\f$
 *    (that is, they are pairwise distinct and take values from
 *    \f$\{0,1,\ldots,n-1\}\f$).
 *  - the differences between adjacent values \f$(d_1,d_2,\ldots,d_{n-1})\f$
 *    with \f$d_i=\operatorname{abs}(x_i-x_{i-1})\f$ form a permutation of
 *    \f$\{1,2,\ldots,n-1\}\f$.
 *
 * See also problem 7 at http://www.csplib.org/.
 *
 * \ingroup ExProblem
 */


class AllInterval : public Script {
private:
  /// The numbers
  IntVarArray x;
public:
	
	enum {
		MODEL_SET,
		MODEL_SYMMETRIC_SET, // series symetriques
		MODEL_CHORD, // variante
		MODEL_SYMMETRIC_CHORD, // variante a la Carter
		MODEL_PARALLEL_CHORD, // variante a la Carter
		MODEL_SET_CHORD, // intersection
		MODEL_SSET_CHORD // intersection symetrique

	};
	
  /// Actual model
  AllInterval(const SizeOptions& opt) :
  	Script(opt),
    x(*this, opt.size(), 0, 66) { // 66 or opt.size() - 1
		const int n = x.size();
		
		IntVarArgs d(n-1);
		IntVarArgs dd(66);
		IntVarArgs xx_(n); // pitch class for AllInterval Chords 
		IntVar douze;
        Rnd r(1U);
		
	if ((opt.model() == MODEL_SET) || (opt.model() == MODEL_SET_CHORD) || (opt.model() == MODEL_SSET_CHORD) ||(opt.model() == MODEL_SYMMETRIC_SET)) // Modele original : serie
		{
			// Set up variables for distance
			for (int i=0; i<n-1; i++)
				d[i] = expr(*this, abs(x[i+1]-x[i]), opt.icl());
			
			// Constrain them to be between 1 and n-1
			dom(*this, d, 1, n-1); 
			dom(*this, x, 0, n-1); 
			
			if((opt.model() == MODEL_SET_CHORD) || (opt.model() == MODEL_SSET_CHORD))
			{
				/*expr(*this,dd[0]==0);
				// Set up variables for distance
				for (int i=0; i<n-1; i++)
				{
					 expr(*this, dd[i+1] == (dd[i]+d[i])%12, opt.icl());
				}
				
				// Constrain them to be between 1 and n-1
				dom(*this, dd,0, n-1); 
				distinct(*this, dd, opt.icl());
				 */
				
				rel(*this, abs(x[0]-x[n-1]) == 6, opt.icl());
				
				
			}
		 
			
			if(opt.symmetry())
			{
				// Break mirror symmetry (renversement)
				rel(*this, x[0], IRT_LE, x[1]);
				// Break symmetry of dual solution (retrograde de la serie) -> 1928 solutions pour accords de 12 sons
				rel(*this, d[0], IRT_GR, d[n-2]);
			}
			//series symetriques
			if ((opt.model() == MODEL_SYMMETRIC_SET)|| (opt.model() == MODEL_SSET_CHORD))
			{
				rel (*this, d[n/2 - 1] == 6); // pivot = triton
				for (int i=0; i<(n/2)-2; i++)
					rel(*this,d[i]+d[n-i-2]==12);
			}
		}
				
else 			
		{
			for (int j=0; j<n; j++)
				xx_[j] = expr(*this, x[j] % 12);
			
			dom(*this, xx_, 0, 11);
			distinct(*this, xx_, opt.icl());
			
			//intervalles
			for (int i=0; i<n-1; i++)
				d[i] =  expr(*this,x[i+1] - x[i],opt.icl());
			dom(*this, d, 1, n-1); 
			
			
			dom(*this, x, 0, n * (n - 1) / 2.); 
			
			//d'autres choses dont on est certain (contraintes redondantes) :
			
			rel(*this, x[0] == 0);
			rel(*this, x[n-1] ==  n * (n - 1) / 2.);
			
			//  break symmetry of dual solution (renversement de l'accord) 
			if(opt.symmetry())
				rel(*this, d[0], IRT_GR, d[n-2]);
			
			//accords symetriques
			if (opt.model() == MODEL_SYMMETRIC_CHORD)
			{
				rel (*this, d[n/2 - 1] == 6); // pivot = triton
				for (int i=0; i<(n/2)-2; i++)
					rel(*this,d[i]+d[n-i-2]==12);
			}
			if (opt.model() == MODEL_PARALLEL_CHORD)
			{
				rel (*this, d[n/2 - 1] == 6); // pivot = triton
				for (int i=0; i<(n/2)-2; i++)
					rel(*this,d[i]+d[n/2 + i]==12);
			}
		}

		distinct(*this, x, opt.icl());
		distinct(*this, d, opt.icl());

#if 0		
		//TEST
		IntVarArray counter(*this,12,0,250);
		IntVar testcounter(*this,0,250);
		
		for (int i=1; i<11; i++) {
			count(*this, d, i,IRT_EQ,testcounter,opt.icl()); // OK
		}
		
		count(*this, d, counter,opt.icl()); // OK
#endif
		//END TEST
		
		if(opt.branching() == 0)
			branch(*this, x, INT_VAR_SIZE_MIN(), INT_VAL_SPLIT_MIN());
		else 
			branch(*this, x, INT_VAR_RND(r), INT_VAL_RND(r));		
	}
	
  /// Constructor for cloning \a e
  AllInterval(bool share, AllInterval& e)
    : Script(share, e) {
    x.update(*this, share, e.x);
  }
  /// Copy during cloning
  virtual Space*
  copy(bool share) {
    return new AllInterval(share, *this);
  }
  /// Print solution
  virtual void
	print(std::ostream& os) const {
#if 0
		std::ostringstream outputstring1;
#endif
		const int n = x.size();
		os << "\tx[" << n << "] = {";
		outputstring << "(";
		for (int i = 0; i < n-1; i++)
		{
			os << x[i] << "(" << abs(x[i+1].val()-x[i].val()) << "),";
			outputstring << x[i] << " ";
	//			outputstring << x[i] ;
		}
		outputstring  << x[n-1] << ")";
		
		os << x[n-1] << "}" << std::endl;
		
//			  strcpy(result,outputstring.str().c_str());
	}
};


/** \brief Main-function
 *  \relates AllInterval
 */
int main(int argc, char* argv[]){
	SizeOptions opt("AllInterval");
	opt.size(1000);
	opt.iterations(5);
	opt.icl(ICL_BND);
	opt.model(AllInterval::MODEL_SET);
	opt.model(AllInterval::MODEL_SET, "set", "all interval set, as defined in CSPlib, problem n.7");
	opt.model(AllInterval::MODEL_SYMMETRIC_SET, "sset", "symmetric all-interval set");
	opt.model(AllInterval::MODEL_CHORD, "chord", "all-interval chord");
	opt.model(AllInterval::MODEL_SYMMETRIC_CHORD, "schord", "symmetric all-interval chord");
	opt.model(AllInterval::MODEL_PARALLEL_CHORD, "pchord", "parallel all-interval chord, alla Carter");
	opt.model(AllInterval::MODEL_SET_CHORD, "setchord", "intersection between AIC and AIS");
	opt.model(AllInterval::MODEL_SSET_CHORD, "ssetchord", "intersection between SAIC and SAIS");
	
	opt.symmetry(0);
	opt.symmetry(0, "no", "calculate all the solutions");
	opt.symmetry(1, "yes", "calculate only the caracteristic solutions");

	opt.branching(0); //normal
	opt.branching(0, "normal", "normal  branching");
	opt.branching(1, "random", "random  branching");

	
	opt.parse(argc, argv);
	if (opt.size() < 2) {
		std::cerr << "size must be at least 2!" << std::endl;
		return -1;
	}
	
	Script::run<AllInterval,DFS,SizeOptions>(opt);
	return 0;
}

// STATISTICS: example-any
// cf allinterval-omgg.cpp

extern "C" {
	char *allintervalG(int n,int n_solutions, char *filename,int timelimit, int model, int symmetry, int searchengine,char *resultat)
	{ 
		SizeOptions opt("AllInterval");
		std::string chaine;

		
		opt.iterations(5);
		opt.solutions(n_solutions);
		opt.icl(ICL_BND);
		
		// il peut y avoir plusieurs modelisation du même problème dans un script
		opt.model(model);
		opt.model(AllInterval::MODEL_SET, "set", "all interval set, as defined in CSPlib, problem n.7");
		opt.model(AllInterval::MODEL_SYMMETRIC_SET, "sset", "symmetric all-interval set");
		opt.model(AllInterval::MODEL_CHORD, "chord", "all-interval chord");
		opt.model(AllInterval::MODEL_SYMMETRIC_CHORD, "schord", "symmetric all-interval chord");
		opt.model(AllInterval::MODEL_PARALLEL_CHORD, "pchord", "parallel all-interval chord, alla Carter");
		opt.model(AllInterval::MODEL_SET_CHORD, "setchord", "intersection between AIC and AIS");
		opt.model(AllInterval::MODEL_SSET_CHORD, "ssetchord", "intersection between SAIC and SAIS");

		opt.symmetry(symmetry);
	
		// on peut choisir le moteur de recherche appelé :
		//		opt.search(searchengine);
		//		opt.search(Jarrell::SEARCH_BAB,"bab");
		//		opt.search(Jarrell::SEARCH_DFS,"dfs");
		//		opt.search(Jarrell::SEARCH_RESTART,"restart");
		//		opt.search(Jarrell::SEARCH_BEST,"best");
		
		opt.time(timelimit);
		
		opt.size(n);		
		
		if (opt.size() < 2) {
			std::cerr << "size must be at least 2!" << std::endl;
			return NULL;
		}		
		strcpy(result,"nil");
		outputstring.str("x");
		outputstring << "(";
		
		
		if (filename)
			output_file.open(filename);
		else
			output_file.open("/Users/lemouton/Desktop/tmp",std::ios::out);
		
		if (output_file.is_open())
		{
			std::cerr << "output in result.tmp" << std::endl;
			output_file << ";; all-interval results : nombre de cellules \n";
		}	
		else			
			std::cerr << "can't open " << filename << std::endl;
		
		switch (opt.search()){
			case SEARCH_DFS:
				Script::run<AllInterval,DFS,SizeOptions>(opt);break;
			case SEARCH_BAB:
				Script::run<AllInterval,BAB,SizeOptions>(opt);break;
//			case SEARCH_RESTART: 
//				Script::run<AllInterval,Restart,SizeOptions>(opt);break;
// this search method is no longer supported since gecode4.0 : use the rBS commandline option instead
			case SEARCH_BEST:
				//		Script::run<AllInterval,bab,SizeOptions>(opt);
				break;
		}		
		
		outputstring << ")";
		//		std::cerr << "result " << outputstring.str().c_str() << std::endl;
		std::cerr << "length " << outputstring.str().size() << " " <<MAXSTRINGLENGTH << std::endl;
#if 1
		strcpy(result,outputstring.str().c_str());
		
		//		std::cerr << "result " << result << std::endl;
		
		// pour éviter de déborder la chaine de sortie:
		
		chaine = outputstring.str();
		
		if (chaine.size() > MAXSTRINGLENGTH) {
			std::cerr << "result string is too long - ";
			chaine.resize(MAXSTRINGLENGTH);
		}
		
		strcpy(result,chaine.c_str());		
		
		// pour éviter de déborder la chaine de sortie(fin
		
		sprintf(resultat, "%s", result);
		
		std::cerr << "resultat " << resultat << std::endl;
#endif		
		output_file.close();
		return result;
	}
	
};

/*to test*/
int
main_to_test_all_interval(int argc, char* argv[]){
	char *a = "test-allinterval";
		char b[200000];
	allintervalG(12,0,a,0,2,0,0,b);
	std::cerr << "resultat " << b << std::endl;

	return 0;
}



