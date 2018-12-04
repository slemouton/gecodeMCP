/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main author:
 *     Serge Lemouton
 *
 *  Copyright:
 *     Serge Lemouton, 2010
 *
 *  Last modified:
 *  This file use Gecode, the generic constraint
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
//#include <gecode/graph.hh>

#include <cstdlib>
#include <fstream>
using namespace std;

#include "chsort.h"
int tester2();

const int nmax = 4096;
const int mmax = 32;

int t_m_[nmax * nmax]; // flat matrix
//int t_mm_[nmax * nmax]; // flat matrix for the path model
int distmax;  // pourrait servir pour trouver les deux accords les plus eloignés
int chords[nmax][mmax];
int chords_size[nmax];
int model_flag; // just for print() !

using namespace Gecode;

/*
 * definition du probleme :
 * trouver l'ordre d'enchainement des accords du repertoire en entree qui minimise les distances entre tous les accords
 * modele 1 : la distance est definie comme le nombre de notes differents
 */


void circuit2(Home home, const IntArgs& c, 
			  const IntVarArgs& x, //solution
			  const IntVarArgs& y, //costs
			  IntVar z, 
			  IntPropLevel icl) {
    int np = y.size();
	int n = sqrt(c.size());
	
	IntArgs mask(np);
//    if ((y.size() != n) || (c.size() != n*n))
//		throw Int::ArgumentSizeMismatch("Graph::circuit");
//    circuit(home, x, icl);
    if (home.failed()) return;
    IntArgs cx(n);
	expr(home,element(mask,np)=0);
	  for (int i=0; i<np-1 ;i++ )
		  mask[i]=1;
	
    for (int i=0; i<np;i++ ) {
		for (int j=0; j<n; j++)
			cx[j] = c[i*n+j];
		
		element(home, cx, x[i], y[i]);
		
    }
#if 1
    linear(home,mask, y ,IRT_EQ, z);
#endif
}


class ChsortOptions : public SizeOptions

{
private:
	/// Maximum distance
	Driver::UnsignedIntOption _distance; // private ?
	/// result length
	Driver::UnsignedIntOption _path_l;   // private ?
	
public:
	
	std::string _repertoire ; // Parameters to be given on command line
	
	
	/// Derive chords from input parameter as a (lisp-like) strings
	void derive(void) {
		int k = 0;
		
		std::cout << "compute problem input data : " << _repertoire << " - " << std::endl;
		
		// 2.parse and calculate cells and intervals :
		_repertoire.erase(0,1);
		
		//		std::cout << "before processing : " << _repertoire << std::endl;
		
		//erase extra spaces
		for(int i = _repertoire.find(" ", 0); i != -1; i = _repertoire.find(" ", i))
		{
			if ((_repertoire[i-1] == ' ') or (_repertoire[i-1] == '(') or (_repertoire[i-1] == ')')or (_repertoire[i+1] == ')'))  //supprime les espaces en trop
				_repertoire.erase(i,1);
			else
				i++;  
		}
		//		std::cout << "after processing : " << _repertoire << std::endl;
		std::cout << "------------------------"<< std::endl;
		
		for(int i = _repertoire.find("(", 0); i != std::string::npos; i = _repertoire.find("(", i))
		{
			int next = 0,sub_next = 0,cell_int,kk=0,fin = 0;
			std::string sous_chaine;
			next = _repertoire.find(")", i);
			sous_chaine = _repertoire.substr(i,next - i);
			i = next;
			
			//	sub_next = sous_chaine.find("(", 1);
			//	std::istringstream buffer(sous_chaine.substr(1,sub_next - 1));
			
			for(int ii = sub_next+1 ; ii != std::string::npos; ii = sous_chaine.find(" ", ii))
			{
				sub_next = sous_chaine.find(" ", ii);
				if (ii == sub_next)
					fin = std::string::npos;
				else
					fin = sub_next - ii;
				std::istringstream buffer(sous_chaine.substr(ii,fin));
				buffer >> cell_int;
				chords[k][kk++] = cell_int;
				//				std::cout <<  k << "/" << kk << "->" << cell_int <<  " ";
				
				ii++;
			}
			chords_size[k] = kk;
			k++;	
		}
		//DEBUGGING 
#if 0
		std::cout << "nchords " << k << std::endl;
		
		for (int i=0;i<k ;i++)
		{
			std::cout <<  " : " << chords[i][0] << " " << chords_size[i]<< std::endl;
		}
#endif
	}
	
	void compute_dist_matrix(void)
	{
		distmax = 0;
		for (int i=0;i < size() ; i++) 
		{
			for (int i2=0;i2 < size() ; i2++) 
			{ int count = 0;
				int distance;
				
				{
					for (int j=0; j < chords_size[i] ; j++)
					{
						for (int k=0; k < chords_size[i2]; k++){
							
#if 0
							std::cout << "dbg" <<  i << "/" << j << "->" << chords[i][j] << std::endl;
							std::cout << "      " <<  i2 << "/" << k  << "->" << chords[i2][k] << std::endl;
							
#endif
							if (chords[i][j] == chords[i2][k]) {
								count++;
							}
						}
					}
				}
				
				distance = chords_size[i] + chords_size[i2] - 2 * count;
				distmax=max(distmax,distance);
				
				t_m_[i + (i2 * size())] =  distance;
				
			}
 		}
		std::cout<< std::endl << "distance maximale = "<< distmax << std::endl;
		
	}
	
/*	
	void compute_dist_matrix_plus(void)
	{
		int s = size();
		int s1 = s + 1;
		
		for (int i=0;i < s ; i++) 
		{
			for (int i2=0;i2 < s ; i2++) 
				t_mm_[i + (i2 * s1 )] =  t_m_[i + (i2 * s)];
			t_mm_[i + (s1 * s1)] = 0;
		}
		for (int i2=0;i2 < s ; i2++) 
			t_mm_[s + (i2 * s1)] =  0;	
	}
*/	
	void print_dist_matrix(void)
	
	{
		int s = size();
		std::cout << "------------------------"<< std::endl;
		for (int i=0;i < s ; i++) 
		{
			for (int i2=0;i2 < s ; i2++) 
				std::cout << " "  << t_m_[i + (i2 * s)]<< " ";
			std::cout << std::endl;
			
		}
		std::cout << "------------------------"<< std::endl;
	}
	
/*	void print_dist_matrix_plus(void)
	
	{
		int s = size() + 1;
		std::cout << "------------------------"<< std::endl;
		for (int i=0;i < s ; i++) 
		{
			for (int i2=0;i2 < s ; i2++) 
				std::cout << " "  << t_mm_[i + (i2 * s)]<< " ";
			std::cout << std::endl;
			
		}
		std::cout << "------------------------"<< std::endl;
	}
*/		
	/// Initialize options for example with name \a s
	ChsortOptions(const char* s,
				  const char* _repertoire,unsigned int distance0,unsigned int path_l0)
    : SizeOptions(s) , 
	_distance("-distance","maximum distance",distance0),
	_path_l("-pathlength","result length",path_l0)
	{		
		add(_distance);
		add(_path_l);
		
	}
	/// Return maximum distance
	unsigned int distance(void) const { return _distance.value(); }
	unsigned int psize(void) const { return _path_l.value(); }
	
	/// Parse options from arguments \a argv (number is \a argc)
	void parse(int& argc, char* argv[]) {
		//		if (argc < 4) 
		//			return;
		if (!strcmp(argv[1],"-help") || !strcmp(argv[1],"--help") || !strcmp(argv[1],"-?"))
		{
			help();
			exit(EXIT_SUCCESS);
		}
		
		
		//		std::cout << "-" << argv[1] << "-" << argv[2]<< "-" << argv[3]<< std::endl;
		
		size(atoi(argv[1]));
		_repertoire.append(argv[2]);
		
		// Remove my 3 arguments (size, cells, chord, first and last note)
		argc -= 2;
		for (int i=1; i<argc; i++)
			argv[i] = argv[i+2];
		
		
		// parse other options !!!!!!!!
		Options::parse(argc,argv);
		
		// remplit le tableau des accords
		derive();
		compute_dist_matrix();
//		print_dist_matrix();
//		compute_dist_matrix_plus();
//		print_dist_matrix_plus();
		
	}
	/// Print help message
	virtual void help(void) {
		Options::help();
		std::cerr << "\t(unsigned int) default: " << size() << std::endl
		<< "\t\tparameter size" << std::endl
		
		<< "\t(string) default: " << _repertoire << std::endl
		<< "\t\tparameter cells" << std::endl;
		
	}
};



class ChSort : public IntMinimizeScript {
protected:
	/// The numbers, aretes des successeurs
	IntVarArray x;
	IntVar sigma;             // cout total du circuit ou du chemin
	IntVar start,end;         // debut et fin du chemin (path)
	//	IntVar pathlength;    // longueur du plus long chemin (si ce n'est pas un circuit)
	IntVarArray costs;
	
public:
	
	enum {
		MODEL_DIFF, // minimise le nombre de notes differentes, parcours (ignore la distance entre le dernier et le premier accord)	
		MODEL_CIRC, // circuit, circulaire (compte la distance entre la derniere et le premier accord)	
		MODEL_PATH, // hamiltonian path	
		MODEL_PATH2  // native hamiltonian path (GECODE 3.7)
		
	};
	/// search variants
	enum {SEARCH_DFS,   //use depth first search algorithm
		SEARCH_BAB,     // use branch and bound to optimize
		SEARCH_RESTART,  /// use restart to optimize 
		SEARCH_BEST  /// use restart to optimize 
	};
	
	/// Actual model
	ChSort(const ChsortOptions& opt) :
	IntMinimizeScript(opt),
	x(*this, opt.psize(),0, opt.size()-1),  
	//x(*this, opt.psize(),0, opt.psize()-1),  
	sigma(*this,0,Int::Limits::max),
	start(*this,0,32),
	end(*this,0,32),
	costs(*this, opt.psize(), 0, Int::Limits::max)
	{
		const int n = opt.size();
		const int np = x.size();
		
		//matrice des distances :
		IntArgs dm(n * n,t_m_);
	//	IntArgs dmm((n+1) * (n+1),t_mm_);

		
		model_flag = opt.model();

		
		if (opt.model() == MODEL_DIFF) // on n'utilise pas la contrainte "circuit" ce qui permet de trouver des parcours ouverts et non exhaustifs ! 
			
		{ 
			distinct(*this, x, opt.ipl());
			
			if(opt.symmetry())
			{
				rel(*this, x[1], IRT_LE, x[0]);
			}
			
			Matrix <IntArgs> dmm(dm,n,n);
			for (int i=0;i<np-1;i++) {
				element(*this, dmm, x[i],x[i+1], costs[i],opt.ipl());
				rel(*this, costs[i] != 0,opt.ipl());
			}
			element(*this, dmm, x[np-1],x[0], costs[np-1],opt.ipl());
			rel(*this, costs[np-1] != 0,opt.ipl());
			
			
			IntArgs mask(np);
			
			
			expr(*this,element(mask,np)=0);
			for (int i=np-1; i--; )
				mask[i]=1;
			
			
			linear(*this, mask, costs, IRT_EQ, sigma, opt.ipl());
			
			// aucun pas plus grand que la distance maximale autorisée
			for (int i=0;i<np-1;i++) {
				rel(*this, costs[i] < opt.distance(),opt.ipl());
			}
			
			
		}
		else if (opt.model() == MODEL_CIRC)
			{
				for (int i=n; i--; ) // empeche les distances nulles
					for (int j=n; j--; )
						if (t_m_[i*n + j] == 0)
							rel(*this, x[i], IRT_NQ, j);
				
				// Just assume that the circle starts forwards
				if(opt.symmetry())
				{
					IntVar p0(*this, 0, n-1);
					element(*this, x, p0, 0);
					rel(*this, p0, IRT_LE, x[0]);
				}
				
				// Enforce that the succesors yield a tour with appropriate costs, using the graph::circuit constraint
				circuit(*this, dm, x, costs, sigma, opt.ipl());
				
				// aucun pas plus grand que la distance maximale autorisée

				for (int i=0;i<np-1;i++) {
					rel(*this, costs[i] < opt.distance(),opt.ipl());
				}
				

			}
			else if (opt.model() == MODEL_PATH)
			{
				// empeche les distances nulles
				for (int i=np; i--; ) 
					for (int j=np; j--; )
						if (t_m_[i*np + j] == 0)
							rel(*this, x[i], IRT_NQ, j);
				
				// Just assume that the circle starts forwards
				if(opt.symmetry())
				{
					IntVar p0(*this, 0, np-1);
					element(*this, x, p0, 0);
					rel(*this, p0, IRT_LE, x[0]);
				}
				
				//circuit constraint (without costs)
#if 0				
				circuit2(*this, dm, x, costs, sigma,opt.ipl());
#else				

				circuit(*this,x,opt.ipl());
				IntArgs cx(n);
				IntArgs mask(np);
				
				
				expr(*this,element(mask,np)=0);
				for (int i=np-1; i--; )
					mask[i]=1;
				
				for (int i=0; i<np; i++) {
					for (int j=0; j<n; j++) {
						cx[j] = dm[i*n+j];
					}
					element(*this, cx, x[i], costs[i]);
				}
				
				
				linear(*this, mask,costs, IRT_EQ, sigma);
				
				// aucun pas plus grand que la distance maximale autorisée
				for (int i=0;i<np-1;i++) {
					rel(*this, costs[i] < opt.distance(),opt.ipl());
				}
				
				
#endif
				
			}
			else if (opt.model() == MODEL_PATH2)
		
			{
#if 0
				for (int i=n; i--; ) // empeche les distances nulles
					for (int j=n; j--; )
						if (t_m_[i*n + j] == 0)
							rel(*this, x[i], IRT_NQ, j);
				
				// Just assume that the circle starts forwards
				if(opt.symmetry())
				{
					IntVar p0(*this, 0, n-1);
					element(*this, x, p0, 0);
					rel(*this, p0, IRT_LE, x[0]);
				}
			
#endif
				// path constraint
				path(*this, dm, x,start,end,costs, sigma, opt.ipl());
//				circuit(*this, dm, x,costs, sigma, opt.ipl());

				
			}
		
			
			// aucun pas plus grand que la distance maximale autorisée
		//	rel(*this,max(costs) < opt.distance());

		// First enumerate cost values, prefer those that maximize cost reduction
		branch(*this, costs, INT_VAR_REGRET_MAX_MAX(), INT_VAL_SPLIT_MIN());
		
		// Then fix the remaining successors
		branch(*this, x,  INT_VAR_MIN_MIN(), INT_VAL_MIN());
		
	}
	///return cost
	virtual IntVar cost(void) const {
		return sigma;
	}
	/// Constructor for cloning \a e
	ChSort(ChSort& e)
    : IntMinimizeScript( e) {
		x.update(*this, e.x);
		sigma.update(*this, e.sigma);
		costs.update(*this, e.costs);
	}
	/// Copy during cloning
	virtual Space*
	copy(void) {
		return new ChSort(*this);
	}
	/// Print solution
	virtual void
	print(std::ostream& os) const {
		
		const int n = x.size();
		bool assigned = true;
		int maxdist = 0,maxpos = 0;
		
		
		os << "\tx[" << n << "] = {";
		for (int i = 0; i < n; i++)
		{
			os << x[i]  << ",";
			os << "[" << costs[i]  << "],";
		}
		
		os  << "}" << std::endl;
		
		os << "sigma = " << sigma << std::endl;
		
		//	if (opt.model() == MODEL_DIFF) // trouve la distance maximale du parcours pour la mettre aux extremes
		for (int i=0;i<n;i++)
			if (costs[i].val()>maxdist) {
				maxdist=costs[i].val();
				maxpos = i;
			}
		os << "maxdist = " << maxdist <<" ("<<maxpos<<")" << std::endl;
		
		if (model_flag == MODEL_DIFF)
		{
			outputstring << "(";
			for(int i = n ; i--;)
				outputstring <<  x[i] << " ";
			outputstring  << ")";
		}
		
		else
		{
#if 1 //if (opt.model() == MODEL_CIRCUIT)
			
				
			outputstring << "(";
			
			for (int i=0; i < n; i++) {
				if (!x[i].assigned()) {
					assigned = false;
					break;
				}
			}
			if (assigned) {
				os << "\tTour: ";
				int j=x[maxpos].val();
				for(int i = n ; i--;)
				{
					os << j << " -> ";
					outputstring << j << " ";
					j=x[j].val();
				} 
				os << std::endl;
				os << "\tCost: " << sigma << std::endl;
			} else {
				os << "\t(Tour): " << std::endl;
				for (int i=0; i<x.size(); i++) {
					os << "\t" << i << " -> " << x[i] << std::endl;
				}
				os << "\tCost: " << sigma << std::endl;
			}
			
			outputstring  << ")";
#endif
		}
		
	}
};


/** \brief Main-function
 *  \relates AllInterval
 */
int main(int argc, char* argv[]){
	ChsortOptions opt("ChSort","((60 61)(61 62))",20,4);
	opt.size(1000);
	opt.iterations(5);
	opt.ipl(IPL_DOM);
	opt.model(ChSort::MODEL_DIFF);
	opt.model(ChSort::MODEL_DIFF,"diff","minimize le nombre de notes differentes");
	opt.model(ChSort::MODEL_CIRC,"circuit","consider distance between last and first item");
	opt.model(ChSort::MODEL_PATH,"path","do not consider the distance between last and first item");
	opt.model(ChSort::MODEL_PATH2, "path2", "gecode native path");
		
	opt.symmetry(0);
	opt.symmetry(0, "no", "calcule toutes les solutions");
	opt.symmetry(1, "yes", "ne calcule que les solutions croissantes");
	
	opt.search(ChSort::SEARCH_BAB);
	opt.search(ChSort::SEARCH_BAB,"bab");
	opt.search(ChSort::SEARCH_DFS,"dfs");
	opt.search(ChSort::SEARCH_RESTART,"restart");
	opt.search(ChSort::SEARCH_BEST,"best");
	
	opt.parse(argc, argv);
	if (opt.size() < 2) {
		std::cerr << "size must be at least 2!" << std::endl;
		return -1;
	}
	
	std::cerr << "model " << opt.model() << std::endl;
	std::cerr << "length = " << opt.psize() << std::endl;
	std::cerr << "time " << opt.time() << std::endl;

	
	switch (opt.search()){
		case ChSort::SEARCH_DFS:
			IntMinimizeScript::run<ChSort,DFS,ChsortOptions>(opt);
			break;
		case ChSort::SEARCH_BAB:
			IntMinimizeScript::run<ChSort,BAB,ChsortOptions>(opt);
			break;
	}
	return 0;
}




extern "C" {
	char *chsortG(int n,const char *str,int n_solutions,int timelimit, int model,int path_length,int distance,char *resultat)
	{ 

        ChsortOptions opt("Chsort","((60 61)(61 62))",distance,path_length);
		std::string chaine;
		
		opt.size(n);
		opt.iterations(5);
		opt.solutions(n_solutions);
		opt.ipl(IPL_DOM);
		
		// il peut y avoir plusieurs modelisation du même problème dans un script
		
		if (model == 0)opt.model(ChSort::MODEL_DIFF);
		if (model == 1)opt.model(ChSort::MODEL_CIRC);
		if (model == 2)opt.model(ChSort::MODEL_PATH);
		if (model == 3)opt.model(ChSort::MODEL_PATH2);


		
		opt.symmetry(0);
	//	opt.symmetry(0, "no", "calcule toutes les solutions");
	//	opt.symmetry(1, "yes", "ne calcule que les solutions croissantes");

		opt.search(ChSort::SEARCH_BAB);
	//	opt.search(ChSort::SEARCH_BAB,"bab");
	//	opt.search(ChSort::SEARCH_DFS,"dfs");
	//	opt.search(ChSort::SEARCH_RESTART,"restart");
	//	opt.search(ChSort::SEARCH_BEST,"best");
		
		
		opt.time(timelimit);		
//		opt._repertoire.append(str);
		opt._repertoire.assign(str);
		
		opt.derive();
		opt.compute_dist_matrix();
		
		if (opt.size() < 2) {
			std::cerr << "size must be at least 2!" << std::endl;
			return NULL;
		}		
		strcpy(result,"nil");
		outputstring.str("x");
		outputstring << "(";
		
		
        std::cerr << "Running chsort - paris " << n << " "<< str << " "<<n_solutions << " "<< timelimit << " "<< model << " " << path_length<< " "<< distance << std::endl;
        
        std::cerr << "n " << n << std::endl;
		std::cerr << "str = " << str << std::endl;
		std::cerr << "solutions " << n_solutions << std::endl;
		std::cerr << "timelimit " << timelimit << std::endl;
		std::cerr << "model " << opt.model() << std::endl;
		std::cerr << "length = " << opt.psize() << std::endl;
		std::cerr << "time " << opt.time() << std::endl;
		std::cerr << "distance " << distance << std::endl;
		
		if (opt.model() == ChSort::MODEL_DIFF)
		std::cerr << "diff " << std::endl;
		else
		std::cerr << "nodiff " << std::endl;
		
		switch (opt.search()){
			case ChSort::SEARCH_DFS:
				IntMinimizeScript::run<ChSort,DFS,ChsortOptions>(opt);
				break;
			case ChSort::SEARCH_BAB:
				IntMinimizeScript::run<ChSort,BAB,ChsortOptions>(opt);
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
		return result;
	}
	
};

/*to test*/
 int main_test_chsort( int argc, char* argv[]){
     char b[200000];
     //	chsortG(10,"((1 2) (2 3) (3 4) (4 5) (5 6) (6 7) (7 8) (8 9) (9 10) (10 11))",0, 1000,3,5,100,b);
     //	chsortG(10,"((1 2)  (3 4) (2 3)(4 5) (5 6) (6 7) (7 8) (8 9) (9 10) (10 11))",1, 1000,0,10,18,b);
     //	chsortG(4,"((60 61)(62 63)(61 62)(60 63))",1,0,0,3,4,b);
     
     //	chsortG(32," ((21 22 29 38 44 54 59 61 72 76 79 87) (21 22 30 32 37 48 52 55 62 71 77 87) (21 22 30 35 37 41 44 50 60 67 76 87) (21 23 24 30 34 37 44 53 64 74 79 87) (21 23 31 36 42 46 49 56 65 76 86 87) (21 25 28 30 35 36 46 53 62 68 79 87) (21 25 28 34 42 44 55 62 71 72 77 87) (21 25 28 35 44 55 65 66 72 74 82 87) (21 25 28 35 44 55 65 70 72 78 86 87) (21 25 28 35 44 55 65 70 78 84 86 87) (21 25 28 36 38 44 55 65 70 71 78 87) (21 26 28 29 35 43 54 61 70 80 84 87) (21 26 34 35 37 41 44 54 60 67 76 87) (21 26 34 36 37 43 53 64 68 71 78 87) (21 28 37 38 43 53 59 70 78 80 84 87) (21 28 37 42 48 59 67 68 70 74 77 87) (21 28 37 43 54 59 60 68 70 74 77 87) (21 28 37 48 54 59 67 68 70 74 77 87) (21 29 30 36 40 43 50 59 70 80 85 87) (21 29 31 42 49 58 59 64 74 80 84 87) (21 29 34 36 40 43 44 50 61 71 78 87) (21 29 40 47 56 66 70 73 79 84 86 87) (21 31 36 38 49 53 56 64 70 71 78 87) (21 31 37 38 46 48 53 64 68 71 78 87) (21 31 38 47 58 64 65 73 78 80 84 87) (21 31 42 46 49 56 65 71 76 84 86 87) (21 32 37 38 46 48 52 55 65 71 78 87) (21 32 40 41 43 47 50 60 66 73 82 87) (21 32 40 42 46 49 55 62 71 72 77 87) (21 32 40 46 50 53 55 60 61 71 78 87) (21 32 42 47 48 55 64 70 74 77 85 87) (21 32 42 49 58 64 72 77 79 83 86 87))",\
     //			1,1000,3,10,18,b);
     
    chsortG(24,"((36 47 49 58 62 69 75 80 88 91 101 102) (36 44 49 58 59 69 75 77 88 91 98 102) (36 44 51 53 64 67 73 82 83 93 98 102) (36 45 47 58 62 67 73 80 88 89 99 102) (36 46 47 56 61 69 75 79 86 89 100 102) (36 46 47 56 63 67 73 81 86 89 100 102) (36 40 51 53 62 67 73 80 83 93 94 102) (36 41 50 52 63 67 73 81 82 92 95 102) (36 43 47 56 58 69 75 76 86 89 97 102) (36 43 51 52 62 65 71 80 82 93 97 102) (36 44 49 52 63 65 71 81 82 91 98 102) (36 45 46 56 61 65 71 79 86 88 99 102) (36 39 50 52 59 67 73 77 82 92 93 102) (36 40 47 56 57 67 73 75 86 89 94 102) (36 41 45 56 58 67 73 76 86 87 95 102) (36 41 49 52 62 63 69 80 82 91 95 102) (36 43 46 56 57 65 71 75 86 88 97 102) (36 44 45 55 58 65 71 76 85 87 98 102) (36 38 49 52 57 65 71 75 82 91 92 102) (36 38 49 52 59 63 69 77 82 91 92 102) (36 39 49 50 58 65 71 76 80 91 93 102) (36 40 45 55 56 65 71 74 85 87 94 102) (36 40 47 50 61 63 69 79 80 89 94 102) (36 37 47 50 58 63 69 76 80 89 91 102))",0, 30000,2,24,16,b);
     
    std::cerr << "resultat " << b << std::endl;
     
     return 0;
 }

#if 0
int
tester2(){
    char b[200000];
    //	chsortG(10,"((1 2) (2 3) (3 4) (4 5) (5 6) (6 7) (7 8) (8 9) (9 10) (10 11))",0, 1000,3,5,100,b);
    //	chsortG(10,"((1 2)  (3 4) (2 3)(4 5) (5 6) (6 7) (7 8) (8 9) (9 10) (10 11))",1, 1000,0,10,18,b);
	//	chsortG(4,"((60 61)(62 63)(61 62)(60 63))",1,0,0,3,4,b);
    
    //	chsortG(32," ((21 22 29 38 44 54 59 61 72 76 79 87) (21 22 30 32 37 48 52 55 62 71 77 87) (21 22 30 35 37 41 44 50 60 67 76 87) (21 23 24 30 34 37 44 53 64 74 79 87) (21 23 31 36 42 46 49 56 65 76 86 87) (21 25 28 30 35 36 46 53 62 68 79 87) (21 25 28 34 42 44 55 62 71 72 77 87) (21 25 28 35 44 55 65 66 72 74 82 87) (21 25 28 35 44 55 65 70 72 78 86 87) (21 25 28 35 44 55 65 70 78 84 86 87) (21 25 28 36 38 44 55 65 70 71 78 87) (21 26 28 29 35 43 54 61 70 80 84 87) (21 26 34 35 37 41 44 54 60 67 76 87) (21 26 34 36 37 43 53 64 68 71 78 87) (21 28 37 38 43 53 59 70 78 80 84 87) (21 28 37 42 48 59 67 68 70 74 77 87) (21 28 37 43 54 59 60 68 70 74 77 87) (21 28 37 48 54 59 67 68 70 74 77 87) (21 29 30 36 40 43 50 59 70 80 85 87) (21 29 31 42 49 58 59 64 74 80 84 87) (21 29 34 36 40 43 44 50 61 71 78 87) (21 29 40 47 56 66 70 73 79 84 86 87) (21 31 36 38 49 53 56 64 70 71 78 87) (21 31 37 38 46 48 53 64 68 71 78 87) (21 31 38 47 58 64 65 73 78 80 84 87) (21 31 42 46 49 56 65 71 76 84 86 87) (21 32 37 38 46 48 52 55 65 71 78 87) (21 32 40 41 43 47 50 60 66 73 82 87) (21 32 40 42 46 49 55 62 71 72 77 87) (21 32 40 46 50 53 55 60 61 71 78 87) (21 32 42 47 48 55 64 70 74 77 85 87) (21 32 42 49 58 64 72 77 79 83 86 87))",\
    //			1,1000,3,10,18,b);
    
    chsortG(24,"((36 47 49 58 62 69 75 80 88 91 101 102) (36 44 49 58 59 69 75 77 88 91 98 102) (36 44 51 53 64 67 73 82 83 93 98 102) (36 45 47 58 62 67 73 80 88 89 99 102) (36 46 47 56 61 69 75 79 86 89 100 102) (36 46 47 56 63 67 73 81 86 89 100 102) (36 40 51 53 62 67 73 80 83 93 94 102) (36 41 50 52 63 67 73 81 82 92 95 102) (36 43 47 56 58 69 75 76 86 89 97 102) (36 43 51 52 62 65 71 80 82 93 97 102) (36 44 49 52 63 65 71 81 82 91 98 102) (36 45 46 56 61 65 71 79 86 88 99 102) (36 39 50 52 59 67 73 77 82 92 93 102) (36 40 47 56 57 67 73 75 86 89 94 102) (36 41 45 56 58 67 73 76 86 87 95 102) (36 41 49 52 62 63 69 80 82 91 95 102) (36 43 46 56 57 65 71 75 86 88 97 102) (36 44 45 55 58 65 71 76 85 87 98 102) (36 38 49 52 57 65 71 75 82 91 92 102) (36 38 49 52 59 63 69 77 82 91 92 102) (36 39 49 50 58 65 71 76 80 91 93 102) (36 40 45 55 56 65 71 74 85 87 94 102) (36 40 47 50 61 63 69 79 80 89 94 102) (36 37 47 50 58 63 69 76 80 89 91 102))",1, 10000,0,8,20,b);
    
	std::cerr << "resultat " << b << std::endl;
    
	return 0;
}
#endif

/******************
 
 ./chsort 10 "((1 2)(4 5) (5 6) (2 3 ) (3 4) (6 7) (7 8) (8 9) (9 10) (10 11))" -solutions 1 -time 1000 -model diff -pathlength 10 -distance 18

 tests :
 passe :
 chsort 72 "((24 34 45 53 62 67 73 80 83 87 88 90) (24 35 43 52 62 69 75 80 82 85 89 90) (24 32 43 52 59 69 75 77 82 85 86 90) (24 33 41 52 62 67 73 80 82 83 87 90) (24 34 41 50 61 69 75 79 80 83 88 90) (24 34 45 50 59 67 73 77 80 87 88 90) (24 35 44 52 57 67 73 75 82 86 89 90) (24 35 45 50 58 67 73 76 80 87 89 90) (24 31 41 50 58 69 75 76 80 83 85 90) (24 32 41 52 57 67 73 75 82 83 86 90) (24 33 40 50 61 65 71 79 80 82 87 90) (24 33 44 52 59 61 67 77 82 86 87 90) (24 34 39 50 59 67 73 77 80 81 88 90) (24 34 45 49 56 65 71 74 79 87 88 90) (24 34 45 53 56 61 67 74 83 87 88 90) (24 35 45 50 58 61 67 76 80 87 89 90) (24 29 39 50 58 67 73 76 80 81 83 90) (24 32 41 46 57 67 73 75 76 83 86 90) (24 32 43 52 57 59 65 75 82 85 86 90) (24 33 41 46 56 67 73 74 76 83 87 90) (24 33 43 50 58 59 65 76 80 85 87 90) (24 34 39 47 56 67 73 74 77 81 88 90) (24 34 45 49 56 59 65 74 79 87 88 90) (24 34 45 50 53 61 67 71 80 87 88 90) (24 35 43 46 56 63 69 74 76 85 89 90) (24 28 39 49 56 65 71 74 79 81 82 90) (24 29 38 46 57 67 73 75 76 80 83 90) (24 29 39 50 58 61 67 76 80 81 83 90) (24 32 39 49 58 59 65 76 79 81 86 90) (24 32 43 46 53 63 69 71 76 85 86 90) (24 33 40 44 55 65 71 73 74 82 87 90) (24 33 44 46 53 61 67 71 76 86 87 90) (24 34 39 50 53 61 67 71 80 81 88 90) (24 34 41 44 55 63 69 73 74 83 88 90) (24 34 43 50 51 59 65 69 80 85 88 90) (24 28 39 49 56 59 65 74 79 81 82 90) (24 31 40 50 51 59 65 69 80 82 85 90) (24 31 41 44 52 63 69 70 74 83 85 90) (24 32 37 46 57 59 65 75 76 79 86 90) (24 33 43 44 52 59 65 70 74 85 87 90) (24 34 39 47 50 61 67 68 77 81 88 90) (24 34 41 50 51 55 61 69 80 83 88 90) (24 34 43 44 51 59 65 69 74 85 88 90) (24 35 38 46 51 61 67 69 76 80 89 90) (24 35 44 46 51 55 61 69 76 86 89 90) (24 27 35 46 56 61 67 74 76 77 81 90) (24 29 37 46 56 57 63 74 76 79 83 90) (24 32 35 46 51 61 67 69 76 77 86 90) (24 32 39 41 52 61 67 70 71 81 86 90) (24 32 39 49 52 53 59 70 79 81 86 90) (24 32 43 46 51 53 59 69 76 85 86 90) (24 35 37 46 50 57 63 68 76 79 89 90) (24 26 37 46 51 59 65 69 76 79 80 90) (24 26 37 46 53 57 63 71 76 79 80 90) (24 27 34 44 55 59 65 73 74 76 81 90) (24 27 38 46 53 55 61 71 76 80 81 90) (24 28 35 44 55 57 63 73 74 77 82 90) (24 28 39 44 53 55 61 71 74 81 82 90) (24 31 39 40 50 59 65 68 70 81 85 90) (24 32 33 43 52 59 65 70 73 75 86 90) (24 32 35 40 51 61 67 69 70 77 86 90) (24 32 37 46 47 57 63 65 76 79 86 90) (24 32 39 40 49 59 65 67 70 81 86 90) (24 32 39 41 52 55 61 70 71 81 86 90) (24 33 35 46 50 55 61 68 76 77 87 90) (24 34 35 44 49 57 63 67 74 77 88 90) (24 34 35 44 51 55 61 69 74 77 88 90) (24 35 39 44 46 55 61 64 74 81 89 90) (24 25 34 44 51 59 65 69 74 76 79 90) (24 25 35 44 52 57 63 70 74 77 79 90) (24 26 33 41 52 61 67 70 71 75 80 90) (24 26 35 46 51 55 61 69 76 77 80 90))"
 passe pas (?) :
 chsort 72 "((24 34 45 53 62 67 73 80 83 87 88 90) (24 35 43 52 62 69 75 80 82 85 89 90) (24 32 43 52 59 69 75 77 82 85 86 90) (24 33 41 52 62 67 73 80 82 83 87 90) (24 34 41 50 61 69 75 79 80 83 88 90) (24 34 45 50 59 67 73 77 80 87 88 90) (24 35 44 52 57 67 73 75 82 86 89 90) (24 35 45 50 58 67 73 76 80 87 89 90) (24 31 41 50 58 69 75 76 80 83 85 90) (24 32 41 52 57 67 73 75 82 83 86 90) (24 33 40 50 61 65 71 79 80 82 87 90) (24 33 44 52 59 61 67 77 82 86 87 90) (24 34 39 50 59 67 73 77 80 81 88 90) (24 34 45 49 56 65 71 74 79 87 88 90) (24 34 45 53 56 61 67 74 83 87 88 90) (24 35 45 50 58 61 67 76 80 87 89 90) (24 29 39 50 58 67 73 76 80 81 83 90) (24 32 41 46 57 67 73 75 76 83 86 90) (24 32 43 52 57 59 65 75 82 85 86 90) (24 33 41 46 56 67 73 74 76 83 87 90) (24 33 43 50 58 59 65 76 80 85 87 90) (24 34 39 47 56 67 73 74 77 81 88 90) (24 34 45 49 56 59 65 74 79 87 88 90) (24 34 45 50 53 61 67 71 80 87 88 90) (24 35 43 46 56 63 69 74 76 85 89 90) (24 28 39 49 56 65 71 74 79 81 82 90) (24 29 38 46 57 67 73 75 76 80 83 90) (24 29 39 50 58 61 67 76 80 81 83 90) (24 32 39 49 58 59 65 76 79 81 86 90) (24 32 43 46 53 63 69 71 76 85 86 90) (24 33 40 44 55 65 71 73 74 82 87 90) (24 33 44 46 53 61 67 71 76 86 87 90) (24 34 39 50 53 61 67 71 80 81 88 90) (24 34 41 44 55 63 69 73 74 83 88 90) (24 34 43 50 51 59 65 69 80 85 88 90) (24 28 39 49 56 59 65 74 79 81 82 90) (24 31 40 50 51 59 65 69 80 82 85 90) (24 31 41 44 52 63 69 70 74 83 85 90) (24 32 37 46 57 59 65 75 76 79 86 90) (24 33 43 44 52 59 65 70 74 85 87 90) (24 34 39 47 50 61 67 68 77 81 88 90) (24 34 41 50 51 55 61 69 80 83 88 90) (24 34 43 44 51 59 65 69 74 85 88 90) (24 35 38 46 51 61 67 69 76 80 89 90) (24 35 44 46 51 55 61 69 76 86 89 90) (24 27 35 46 56 61 67 74 76 77 81 90) (24 29 37 46 56 57 63 74 76 79 83 90) (24 32 35 46 51 61 67 69 76 77 86 90) (24 32 39 41 52 61 67 70 71 81 86 90) (24 32 39 49 52 53 59 70 79 81 86 90) (24 32 43 46 51 53 59 69 76 85 86 90) (24 35 37 46 50 57 63 68 76 79 89 90) (24 26 37 46 51 59 65 69 76 79 80 90) (24 26 37 46 53 57 63 71 76 79 80 90) (24 27 34 44 55 59 65 73 74 76 81 90) (24 27 38 46 53 55 61 71 76 80 81 90) (24 28 35 44 55 57 63 73 74 77 82 90) (24 28 39 44 53 55 61 71 74 81 82 90) (24 31 39 40 50 59 65 68 70 81 85 90) (24 32 33 43 52 59 65 70 73 75 86 90) (24 32 35 40 51 61 67 69 70 77 86 90) (24 32 37 46 47 57 63 65 76 79 86 90) (24 32 39 40 49 59 65 67 70 81 86 90) (24 32 39 41 52 55 61 70 71 81 86 90) (24 33 35 46 50 55 61 68 76 77 87 90) (24 34 35 44 49 57 63 67 74 77 88 90) (24 34 35 44 51 55 61 69 74 77 88 90) (24 35 39 44 46 55 61 64 74 81 89 90) (24 25 34 44 51 59 65 69 74 76 79 90) (24 25 35 44 52 57 63 70 74 77 79 90) (24 26 33 41 52 61 67 70 71 75 80 90) (24 26 35 46 51 55 61 69 76 77 80 90))"
 
 chsort 10 "((1 2) (2 3) (3 4) (4 5) (5 6) (6 7) (7 8) (8 9) (9 10) (10 11))"
 chsort 10 "( (6 7) (7 8) (8 9) (9 10) (10 11)(1 2)(2 3)(3 4)(4 5)(5 6))" -solutions 0 -symmetry no -model circuit
 chsort 10 "((1 2) (2 3) (3 4) (4 5) (5 6) (6 7) (7 8) (8 9) (9 10) (10 11))" -model circuit -pathlength 10
 chsort 10 "((1 2) (2 3) (3 4) (4 5) (5 6) (6 7) (7 8) (8 9) (9 10) (10 11))" -solutions 1 -time 1000 -model diff -pathlength 10 -distance 18
 chsort 10 "((1 2) (2 3) (3 4) (4 5) (5 6) (6 7) (7 8) (8 9) (9 10) (10 11))" -model path -pathlength 10
 chsort 10 "((1 2) (2 3) (3 4) (4 5) (5 6) (6 7) (7 8) (8 9) (9 10) (10 11))" -model path -pathlength 5
 chsort 120 "((0 1 3 7 14 17 23 34 44 52 57 66) (0 1 4 14 22 27 33 44 53 55 59 66) (0 1 5 7 10 15 21 32 40 50 59 66) (0 1 5 7 16 21 27 38 46 56 59 66) (0 1 8 10 14 23 29 40 45 55 63 66) (0 1 8 16 26 29 35 46 51 55 57 66) (0 1 9 19 26 35 41 52 56 58 63 66) (0 1 10 14 21 23 29 40 43 51 56 66) (0 1 10 17 19 27 33 44 47 52 62 66) (0 1 10 20 27 35 41 52 55 57 62 66) (0 1 10 20 28 33 39 50 53 55 59 66) (0 1 11 16 19 27 33 44 46 53 62 66) (0 2 3 7 10 17 23 33 44 52 61 66) (0 2 3 8 16 19 25 35 46 53 57 66) (0 2 7 16 17 21 27 37 44 47 58 66) (0 2 9 13 22 23 29 39 44 52 55 66) (0 2 9 20 28 37 43 53 58 59 63 66) (0 2 10 11 16 25 31 41 45 56 63 66) (0 2 10 17 28 31 37 47 51 56 57 66) (0 2 13 16 23 27 33 43 44 53 58 66) (0 3 4 9 19 23 29 38 49 56 58 66) (0 3 4 14 22 29 35 44 55 57 61 66) (0 3 5 9 20 25 31 40 50 58 59 66) (0 3 5 16 23 31 37 46 56 57 62 66) (0 3 7 8 13 23 29 38 46 57 64 66) (0 3 7 9 10 17 23 32 40 50 61 66) (0 3 8 9 11 19 25 34 41 52 62 66) (0 3 8 10 14 25 31 40 47 57 65 66) (0 3 10 14 16 17 23 32 37 45 55 66) (0 3 10 21 25 35 41 50 55 56 64 66) (0 3 11 16 17 19 25 34 38 45 56 66) (0 3 11 21 26 37 43 52 56 58 65 66) (0 3 13 20 31 35 41 50 52 57 58 66) (0 3 13 21 28 29 35 44 46 50 55 66) (0 3 14 21 29 31 37 46 47 52 56 66) (0 3 14 22 32 37 43 52 53 57 59 66) (0 4 5 14 19 21 27 35 46 49 56 66) (0 4 7 8 10 15 21 29 38 49 59 66) (0 4 9 10 20 29 35 43 50 61 63 66) (0 4 9 11 14 25 31 39 46 56 65 66) (0 4 11 14 25 27 33 41 46 55 56 66) (0 4 13 20 22 33 39 47 50 55 65 66) (0 4 14 19 20 23 29 37 39 46 57 66) (0 4 14 19 22 33 39 47 49 56 65 66) (0 4 14 25 32 41 47 55 57 58 63 66) (0 4 14 25 34 39 45 53 55 56 59 66) (0 4 15 17 26 31 37 45 46 56 59 66) (0 4 15 22 32 35 41 49 50 55 57 66) (0 5 7 8 11 15 21 28 38 49 58 66) (0 5 8 10 14 15 21 28 37 47 55 66) (0 5 9 11 22 31 37 44 52 62 63 66) (0 5 13 23 26 27 33 40 44 46 55 66) (0 5 13 23 32 33 39 46 50 52 55 66) (0 5 14 16 20 21 27 34 37 47 55 66) (0 5 14 16 27 31 37 44 47 57 58 66) (0 5 14 22 33 43 49 56 59 63 64 66) (0 5 14 25 35 39 45 52 55 56 58 66) (0 5 15 23 34 37 43 50 52 56 57 66) (0 5 16 20 22 25 31 38 39 47 57 66) (0 5 16 26 34 43 49 56 57 59 63 66) (0 7 8 10 14 17 23 28 39 49 57 66) (0 7 8 16 26 35 41 46 57 61 63 66) (0 7 9 13 14 23 29 34 44 52 63 66) (0 7 10 11 13 21 27 32 41 52 62 66) (0 7 10 14 15 17 23 28 37 45 56 66) (0 7 10 20 21 29 35 40 49 51 62 66) (0 7 10 20 28 39 45 50 59 61 65 66) (0 7 11 13 16 27 33 38 46 56 65 66) (0 7 11 13 22 33 39 44 52 62 65 66) (0 7 15 25 26 29 35 40 44 46 57 66) (0 7 16 26 34 45 51 56 59 61 65 66) (0 7 17 28 37 45 51 56 58 59 62 66) (0 8 9 14 16 25 31 35 46 53 63 66) (0 8 9 19 22 29 35 39 50 52 61 66) (0 8 10 11 14 21 27 31 41 52 61 66) (0 8 10 11 16 19 25 29 39 50 57 66) (0 8 10 17 26 27 33 37 47 52 55 66) (0 8 10 17 28 37 43 47 57 62 63 66) (0 8 11 16 26 27 33 37 46 53 55 66) (0 8 13 22 23 33 39 43 50 53 64 66) (0 8 15 25 34 35 41 45 50 52 55 66) (0 8 15 26 28 31 37 41 46 47 57 66) (0 8 17 28 38 45 51 55 58 59 61 66) (0 8 19 22 29 39 45 49 50 59 64 66) (0 9 10 14 16 23 29 32 43 51 61 66) (0 9 10 15 19 29 35 38 49 56 64 66) (0 9 11 15 20 31 37 40 50 58 65 66) (0 9 11 16 17 25 31 34 44 51 62 66) (0 9 13 15 22 23 29 32 40 50 55 66) (0 9 13 20 31 41 47 50 58 63 64 66) (0 9 14 15 23 25 31 34 41 52 56 66) (0 9 14 22 32 43 49 52 59 63 65 66) (0 9 16 26 34 35 41 44 49 51 55 66) (0 9 16 27 37 41 47 50 55 56 58 66) (0 9 17 27 38 43 49 52 56 58 59 66) (0 9 17 28 35 37 43 46 50 51 56 66) (0 9 19 20 25 29 35 38 40 51 58 66) (0 9 19 27 28 35 41 44 46 50 61 66) (0 9 20 22 26 31 37 40 41 51 59 66) (0 9 20 27 29 37 43 46 47 52 62 66) (0 10 11 20 25 33 39 41 52 55 62 66) (0 10 14 19 20 29 35 37 45 52 63 66) (0 10 14 25 32 35 41 43 51 52 57 66) (0 10 15 16 20 23 29 31 38 49 57 66) (0 10 15 23 26 37 43 45 52 56 65 66) (0 10 17 20 31 39 45 47 52 61 62 66) (0 10 21 28 32 41 47 49 50 55 63 66) (0 10 21 29 38 43 49 51 52 56 59 66) (0 11 13 20 29 33 39 40 50 55 58 66) (0 11 14 16 20 27 33 34 43 53 61 66) (0 11 14 16 21 25 31 32 41 51 58 66) (0 11 14 19 29 33 39 40 49 56 58 66) (0 11 14 22 27 37 43 44 53 57 64 66) (0 11 15 17 22 25 31 32 40 50 57 66) (0 11 16 20 22 31 37 38 45 53 63 66) (0 11 16 26 34 37 43 44 51 53 57 66) (0 11 19 29 32 39 45 46 50 52 61 66) (0 11 19 29 38 45 51 52 56 58 61 66) (0 11 20 22 26 33 39 40 43 53 61 66) (0 11 21 29 34 43 49 50 52 56 63 66))
 "  -pathlength 32 -model path -solutions 1 -distance 18 -time 5000
 
 ./chsort 120 "((0 1 3 7 14 17 23 34 44 52 57 66) (0 1 4 14 22 27 33 44 53 55 59 66) (0 1 5 7 10 15 21 32 40 50 59 66) (0 1 5 7 16 21 27 38 46 56 59 66) (0 1 8 10 14 23 29 40 45 55 63 66) (0 1 8 16 26 29 35 46 51 55 57 66) (0 1 9 19 26 35 41 52 56 58 63 66) (0 1 10 14 21 23 29 40 43 51 56 66) (0 1 10 17 19 27 33 44 47 52 62 66) (0 1 10 20 27 35 41 52 55 57 62 66) (0 1 10 20 28 33 39 50 53 55 59 66) (0 1 11 16 19 27 33 44 46 53 62 66) (0 2 3 7 10 17 23 33 44 52 61 66) (0 2 3 8 16 19 25 35 46 53 57 66) (0 2 7 16 17 21 27 37 44 47 58 66) (0 2 9 13 22 23 29 39 44 52 55 66) (0 2 9 20 28 37 43 53 58 59 63 66) (0 2 10 11 16 25 31 41 45 56 63 66) (0 2 10 17 28 31 37 47 51 56 57 66) (0 2 13 16 23 27 33 43 44 53 58 66) (0 3 4 9 19 23 29 38 49 56 58 66) (0 3 4 14 22 29 35 44 55 57 61 66) (0 3 5 9 20 25 31 40 50 58 59 66) (0 3 5 16 23 31 37 46 56 57 62 66) (0 3 7 8 13 23 29 38 46 57 64 66) (0 3 7 9 10 17 23 32 40 50 61 66) (0 3 8 9 11 19 25 34 41 52 62 66) (0 3 8 10 14 25 31 40 47 57 65 66) (0 3 10 14 16 17 23 32 37 45 55 66) (0 3 10 21 25 35 41 50 55 56 64 66) (0 3 11 16 17 19 25 34 38 45 56 66) (0 3 11 21 26 37 43 52 56 58 65 66) (0 3 13 20 31 35 41 50 52 57 58 66) (0 3 13 21 28 29 35 44 46 50 55 66) (0 3 14 21 29 31 37 46 47 52 56 66) (0 3 14 22 32 37 43 52 53 57 59 66) (0 4 5 14 19 21 27 35 46 49 56 66) (0 4 7 8 10 15 21 29 38 49 59 66) (0 4 9 10 20 29 35 43 50 61 63 66) (0 4 9 11 14 25 31 39 46 56 65 66) (0 4 11 14 25 27 33 41 46 55 56 66) (0 4 13 20 22 33 39 47 50 55 65 66) (0 4 14 19 20 23 29 37 39 46 57 66) (0 4 14 19 22 33 39 47 49 56 65 66) (0 4 14 25 32 41 47 55 57 58 63 66) (0 4 14 25 34 39 45 53 55 56 59 66) (0 4 15 17 26 31 37 45 46 56 59 66) (0 4 15 22 32 35 41 49 50 55 57 66) (0 5 7 8 11 15 21 28 38 49 58 66) (0 5 8 10 14 15 21 28 37 47 55 66) (0 5 9 11 22 31 37 44 52 62 63 66) (0 5 13 23 26 27 33 40 44 46 55 66) (0 5 13 23 32 33 39 46 50 52 55 66) (0 5 14 16 20 21 27 34 37 47 55 66) (0 5 14 16 27 31 37 44 47 57 58 66) (0 5 14 22 33 43 49 56 59 63 64 66) (0 5 14 25 35 39 45 52 55 56 58 66) (0 5 15 23 34 37 43 50 52 56 57 66) (0 5 16 20 22 25 31 38 39 47 57 66) (0 5 16 26 34 43 49 56 57 59 63 66) (0 7 8 10 14 17 23 28 39 49 57 66) (0 7 8 16 26 35 41 46 57 61 63 66) (0 7 9 13 14 23 29 34 44 52 63 66) (0 7 10 11 13 21 27 32 41 52 62 66) (0 7 10 14 15 17 23 28 37 45 56 66) (0 7 10 20 21 29 35 40 49 51 62 66) (0 7 10 20 28 39 45 50 59 61 65 66) (0 7 11 13 16 27 33 38 46 56 65 66) (0 7 11 13 22 33 39 44 52 62 65 66) (0 7 15 25 26 29 35 40 44 46 57 66) (0 7 16 26 34 45 51 56 59 61 65 66) (0 7 17 28 37 45 51 56 58 59 62 66) (0 8 9 14 16 25 31 35 46 53 63 66) (0 8 9 19 22 29 35 39 50 52 61 66) (0 8 10 11 14 21 27 31 41 52 61 66) (0 8 10 11 16 19 25 29 39 50 57 66) (0 8 10 17 26 27 33 37 47 52 55 66) (0 8 10 17 28 37 43 47 57 62 63 66) (0 8 11 16 26 27 33 37 46 53 55 66) (0 8 13 22 23 33 39 43 50 53 64 66) (0 8 15 25 34 35 41 45 50 52 55 66) (0 8 15 26 28 31 37 41 46 47 57 66) (0 8 17 28 38 45 51 55 58 59 61 66) (0 8 19 22 29 39 45 49 50 59 64 66) (0 9 10 14 16 23 29 32 43 51 61 66) (0 9 10 15 19 29 35 38 49 56 64 66) (0 9 11 15 20 31 37 40 50 58 65 66) (0 9 11 16 17 25 31 34 44 51 62 66) (0 9 13 15 22 23 29 32 40 50 55 66) (0 9 13 20 31 41 47 50 58 63 64 66) (0 9 14 15 23 25 31 34 41 52 56 66) (0 9 14 22 32 43 49 52 59 63 65 66) (0 9 16 26 34 35 41 44 49 51 55 66) (0 9 16 27 37 41 47 50 55 56 58 66) (0 9 17 27 38 43 49 52 56 58 59 66) (0 9 17 28 35 37 43 46 50 51 56 66) (0 9 19 20 25 29 35 38 40 51 58 66) (0 9 19 27 28 35 41 44 46 50 61 66) (0 9 20 22 26 31 37 40 41 51 59 66) (0 9 20 27 29 37 43 46 47 52 62 66) (0 10 11 20 25 33 39 41 52 55 62 66) (0 10 14 19 20 29 35 37 45 52 63 66) (0 10 14 25 32 35 41 43 51 52 57 66) (0 10 15 16 20 23 29 31 38 49 57 66) (0 10 15 23 26 37 43 45 52 56 65 66) (0 10 17 20 31 39 45 47 52 61 62 66) (0 10 21 28 32 41 47 49 50 55 63 66) (0 10 21 29 38 43 49 51 52 56 59 66) (0 11 13 20 29 33 39 40 50 55 58 66) (0 11 14 16 20 27 33 34 43 53 61 66) (0 11 14 16 21 25 31 32 41 51 58 66) (0 11 14 19 29 33 39 40 49 56 58 66) (0 11 14 22 27 37 43 44 53 57 64 66) (0 11 15 17 22 25 31 32 40 50 57 66) (0 11 16 20 22 31 37 38 45 53 63 66) (0 11 16 26 34 37 43 44 51 53 57 66) (0 11 19 29 32 39 45 46 50 52 61 66) (0 11 19 29 38 45 51 52 56 58 61 66) (0 11 20 22 26 33 39 40 43 53 61 66) (0 11 21 29 34 43 49 50 52 56 63 66))
 "-pathlength 10 -model diff -solutions 4 -distance 11 -time 0 -search bab -symmetry yes
->>4 secondes ...
 
 48 series/accords tous intervalles symetriques:
 ./chsort 48 "((36 47 49 58 62 69 75 80 88 91 101 102) (36 47 49 58 62 69 75 80 88 91 101 102) (36 44 49 58 59 69 75 77 88 91 98 102) (36 44 51 53 64 67 73 82 83 93 98 102) (36 45 47 58 62 67 73 80 88 89 99 102) (36 46 47 56 61 69 75 79 86 89 100 102) (36 46 47 56 63 67 73 81 86 89 100 102) (36 44 49 58 59 69 75 77 88 91 98 102) (36 44 51 53 64 67 73 82 83 93 98 102) (36 45 47 58 62 67 73 80 88 89 99 102) (36 46 47 56 61 69 75 79 86 89 100 102) (36 46 47 56 63 67 73 81 86 89 100 102) (36 40 51 53 62 67 73 80 83 93 94 102) (36 41 50 52 63 67 73 81 82 92 95 102) (36 43 47 56 58 69 75 76 86 89 97 102) (36 43 51 52 62 65 71 80 82 93 97 102) (36 44 49 52 63 65 71 81 82 91 98 102) (36 45 46 56 61 65 71 79 86 88 99 102) (36 40 51 53 62 67 73 80 83 93 94 102) (36 41 50 52 63 67 73 81 82 92 95 102) (36 43 47 56 58 69 75 76 86 89 97 102) (36 43 51 52 62 65 71 80 82 93 97 102) (36 44 49 52 63 65 71 81 82 91 98 102) (36 45 46 56 61 65 71 79 86 88 99 102) (36 39 50 52 59 67 73 77 82 92 93 102) (36 40 47 56 57 67 73 75 86 89 94 102) (36 41 45 56 58 67 73 76 86 87 95 102) (36 41 49 52 62 63 69 80 82 91 95 102) (36 43 46 56 57 65 71 75 86 88 97 102) (36 44 45 55 58 65 71 76 85 87 98 102) (36 39 50 52 59 67 73 77 82 92 93 102) (36 40 47 56 57 67 73 75 86 89 94 102) (36 41 45 56 58 67 73 76 86 87 95 102) (36 41 49 52 62 63 69 80 82 91 95 102) (36 43 46 56 57 65 71 75 86 88 97 102) (36 44 45 55 58 65 71 76 85 87 98 102) (36 38 49 52 57 65 71 75 82 91 92 102) (36 38 49 52 59 63 69 77 82 91 92 102) (36 39 49 50 58 65 71 76 80 91 93 102) (36 40 45 55 56 65 71 74 85 87 94 102) (36 40 47 50 61 63 69 79 80 89 94 102) (36 38 49 52 57 65 71 75 82 91 92 102) (36 38 49 52 59 63 69 77 82 91 92 102) (36 39 49 50 58 65 71 76 80 91 93 102) (36 40 45 55 56 65 71 74 85 87 94 102) (36 40 47 50 61 63 69 79 80 89 94 102) (36 37 47 50 58 63 69 76 80 89 91 102) (36 37 47 50 58 63 69 76 80 89 91 102))" -model circuit -pathlength 48 -search bab -solutions 0 -distance 14 -symmetry yes
 -> trouve en 6min30, LA meilleure solution : sigma = 440
 
 chsort 24 " ((36 47 49 58 62 69 75 80 88 91 101 102) (36 44 49 58 59 69 75 77 88 91 98 102) (36 44 51 53 64 67 73 82 83 93 98 102) (36 45 47 58 62 67 73 80 88 89 99 102) (36 46 47 56 61 69 75 79 86 89 100 102) (36 46 47 56 63 67 73 81 86 89 100 102) (36 40 51 53 62 67 73 80 83 93 94 102) (36 41 50 52 63 67 73 81 82 92 95 102) (36 43 47 56 58 69 75 76 86 89 97 102) (36 43 51 52 62 65 71 80 82 93 97 102) (36 44 49 52 63 65 71 81 82 91 98 102) (36 45 46 56 61 65 71 79 86 88 99 102) (36 39 50 52 59 67 73 77 82 92 93 102) (36 40 47 56 57 67 73 75 86 89 94 102) (36 41 45 56 58 67 73 76 86 87 95 102) (36 41 49 52 62 63 69 80 82 91 95 102) (36 43 46 56 57 65 71 75 86 88 97 102) (36 44 45 55 58 65 71 76 85 87 98 102) (36 38 49 52 57 65 71 75 82 91 92 102) (36 38 49 52 59 63 69 77 82 91 92 102) (36 39 49 50 58 65 71 76 80 91 93 102) (36 40 45 55 56 65 71 74 85 87 94 102) (36 40 47 50 61 63 69 79 80 89 94 102) (36 37 47 50 58 63 69 76 80 89 91 102))" -model circuit -pathlength 24 -search bab -solutions 0 -distance 14 -symmetry yes
 -> trouve instantanement LA meilleure solution : sigma = 236

 chsort 24 " ((36 47 49 58 62 69 75 80 88 91 101 102) (36 44 49 58 59 69 75 77 88 91 98 102) (36 44 51 53 64 67 73 82 83 93 98 102) (36 45 47 58 62 67 73 80 88 89 99 102) (36 46 47 56 61 69 75 79 86 89 100 102) (36 46 47 56 63 67 73 81 86 89 100 102) (36 40 51 53 62 67 73 80 83 93 94 102) (36 41 50 52 63 67 73 81 82 92 95 102) (36 43 47 56 58 69 75 76 86 89 97 102) (36 43 51 52 62 65 71 80 82 93 97 102) (36 44 49 52 63 65 71 81 82 91 98 102) (36 45 46 56 61 65 71 79 86 88 99 102) (36 39 50 52 59 67 73 77 82 92 93 102) (36 40 47 56 57 67 73 75 86 89 94 102) (36 41 45 56 58 67 73 76 86 87 95 102) (36 41 49 52 62 63 69 80 82 91 95 102) (36 43 46 56 57 65 71 75 86 88 97 102) (36 44 45 55 58 65 71 76 85 87 98 102) (36 38 49 52 57 65 71 75 82 91 92 102) (36 38 49 52 59 63 69 77 82 91 92 102) (36 39 49 50 58 65 71 76 80 91 93 102) (36 40 45 55 56 65 71 74 85 87 94 102) (36 40 47 50 61 63 69 79 80 89 94 102) (36 37 47 50 58 63 69 76 80 89 91 102))" -model diff -pathlength 10 -search bab -solutions 0 -distance 14 -symmetry yes
 -> trouve en 12 s , LA meilleure solution : sigma = 80
 chsort 24 " ((36 47 49 58 62 69 75 80 88 91 101 102) (36 44 49 58 59 69 75 77 88 91 98 102) (36 44 51 53 64 67 73 82 83 93 98 102) (36 45 47 58 62 67 73 80 88 89 99 102) (36 46 47 56 61 69 75 79 86 89 100 102) (36 46 47 56 63 67 73 81 86 89 100 102) (36 40 51 53 62 67 73 80 83 93 94 102) (36 41 50 52 63 67 73 81 82 92 95 102) (36 43 47 56 58 69 75 76 86 89 97 102) (36 43 51 52 62 65 71 80 82 93 97 102) (36 44 49 52 63 65 71 81 82 91 98 102) (36 45 46 56 61 65 71 79 86 88 99 102) (36 39 50 52 59 67 73 77 82 92 93 102) (36 40 47 56 57 67 73 75 86 89 94 102) (36 41 45 56 58 67 73 76 86 87 95 102) (36 41 49 52 62 63 69 80 82 91 95 102) (36 43 46 56 57 65 71 75 86 88 97 102) (36 44 45 55 58 65 71 76 85 87 98 102) (36 38 49 52 57 65 71 75 82 91 92 102) (36 38 49 52 59 63 69 77 82 91 92 102) (36 39 49 50 58 65 71 76 80 91 93 102) (36 40 45 55 56 65 71 74 85 87 94 102) (36 40 47 50 61 63 69 79 80 89 94 102) (36 37 47 50 58 63 69 76 80 89 91 102))" -model diff -pathlength 12 -search bab -solutions 0 -distance 14 -symmetry yes
 -> trouve en 3min , LA meilleure solution : sigma = 98

 ./chsort 32 " ((21 22 29 38 44 54 59 61 72 76 79 87) (21 22 30 32 37 48 52 55 62 71 77 87) (21 22 30 35 37 41 44 50 60 67 76 87) (21 23 24 30 34 37 44 53 64 74 79 87) (21 23 31 36 42 46 49 56 65 76 86 87) (21 25 28 30 35 36 46 53 62 68 79 87) (21 25 28 34 42 44 55 62 71 72 77 87) (21 25 28 35 44 55 65 66 72 74 82 87) (21 25 28 35 44 55 65 70 72 78 86 87) (21 25 28 35 44 55 65 70 78 84 86 87) (21 25 28 36 38 44 55 65 70 71 78 87) (21 26 28 29 35 43 54 61 70 80 84 87) (21 26 34 35 37 41 44 54 60 67 76 87) (21 26 34 36 37 43 53 64 68 71 78 87) (21 28 37 38 43 53 59 70 78 80 84 87) (21 28 37 42 48 59 67 68 70 74 77 87) (21 28 37 43 54 59 60 68 70 74 77 87) (21 28 37 48 54 59 67 68 70 74 77 87) (21 29 30 36 40 43 50 59 70 80 85 87) (21 29 31 42 49 58 59 64 74 80 84 87) (21 29 34 36 40 43 44 50 61 71 78 87) (21 29 40 47 56 66 70 73 79 84 86 87) (21 31 36 38 49 53 56 64 70 71 78 87) (21 31 37 38 46 48 53 64 68 71 78 87) (21 31 38 47 58 64 65 73 78 80 84 87) (21 31 42 46 49 56 65 71 76 84 86 87) (21 32 37 38 46 48 52 55 65 71 78 87) (21 32 40 41 43 47 50 60 66 73 82 87) (21 32 40 42 46 49 55 62 71 72 77 87) (21 32 40 46 50 53 55 60 61 71 78 87) (21 32 42 47 48 55 64 70 74 77 85 87) (21 32 42 49 58 64 72 77 79 83 86 87))" \
 -pathlength 32 -model circuit -solutions 0 -distance 18 -time 5000
 
 questions ouvertes :
 trouver le circuit le plus long avec aucun pas superieur à une limite fixée : model 3
 trouver le parcours ouvert le plus long avec aucun pas superieur à une limite fixée
 pour cela il ne faudrait pas utiliser graph::circuit qui force à parcourir tous les noeuds

****************/


