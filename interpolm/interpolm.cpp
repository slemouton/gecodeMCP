/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 * Musical constraints problems
 * Melodic (or Matrix) Interpolation
 *   Main author:
 *   Serge Lemouton ircam 2009
 *
 */

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/set.hh>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


#include "interpolm.h"

#define MAXPITCH 128

static char result[8192];

using namespace Gecode;

class Chord {
public:
	int size;
	int octaviation; // 0/1 
	int chord[48];
};


//namespace  {
	
static	Chord chordA1={7,0,{60, 65, 66, 68, 71, 76, 82}}; // modified by arguments
static	Chord startA1={3,0,{12,22,33}};
static	Chord endA1={3,0,{4,5,6}};
static	Chord forbiddenA1={3,0,{4,5,6}};
	
//}

class interpolMOptions : public Options {
private:
	Driver::UnsignedIntOption _steps; //états intermediares
	Driver::UnsignedIntOption _n; //nombres de notes
	Driver::UnsignedIntOption _variante; ///variantes	
	
public:
	Driver::StringValueOption _start; //etat initial
	Driver::StringValueOption _end; //etat final
	Driver::StringValueOption _domain; // "harmonie"
	Driver::StringValueOption _forbiddenI; //intervalles interdits
	
	std::vector<int> itv_vector; // to keep a track of all the required interval
	std::vector<int> forbidden_itv_vector; // to keep a track of all the forbidden interval
	
	//initialize options :
	interpolMOptions(const char *s,char *str1,int steps0 = 3,int n0 = 3)
	: Options(s),
	_steps("-steps","number of intermediary steps", steps0),
	_n("-n","note number",n0),
	_start("-start","initial step",str1),
	_end("-end","final step","(3 2 1)"),
	_domain("-domain","notes autorisées","(1 2 3 4 5 6 7 8)"),
	_forbiddenI("-no","intervalles interdits","( )"),
	_variante("-variante","model : only one for now ...",false)
	{
		// add options
		add(_steps);add(_n);add(_start);add(_end);add(_domain);add(_forbiddenI);add(_variante);
	}
	/// Parse options from arguments \a argv (number is \a argc)
	void parse(int& argc, char* argv[]) {
		Options::parse(argc,argv);
	}
	/// Get v, number of sequences
	int steps(void) const { return _steps.value(); }
	/// Get q, number of symbols
	int n(void) const { return _n.value(); }
	int size(void) const { return steps() * n(); }
	
	bool start(void) const { return  _start.value(); }
	bool end(void) const { return  _end.value(); }
	bool variante(void) const { return _variante.value(); }
	void assign_arg_string(std::string str,Chord* result)
	{	int j=0,chord_note, chord_size = 0;
		
		std::cout << "compute problem input data : " << str << std::endl;
		
		
		// 1.normalisation de la chaine d'entrée (en enlevant simplement les espaces en trop !!!!)
		// + parse and calculate chord :
		
		for(int i = str.find("(", 0); i != -1; i = str.find("(", i))
		{
			str.replace(i,1," ");
			i++;  // Move past the last discovered instance to avoid finding same
		}
		for(int i = str.find(")", 0); i != -1; i = str.find(")", i))
		{
			str.replace(i,1," ");
			i++;  // Move past the last discovered instance to avoid finding same
		}
		
		for(int i = str.find(" ", 0); i != -1; i = str.find(" ", i))
		{
			if (str[i-1] == ' ')  //supprime les espaces en trop
			{
				str.erase(i,1);
				i--;
			} 
			i++;  
		}
		
		for(int i = str.find(" ", 0); i != std::string::npos; i = str.find(" ", i))
		{
			int next = 0;
			chord_size++;
			i++;  // Move past the last discovered instance to avoid finding same
			next = str.find(" ", i);
			//	std::cout << "next,i"  << next << ","<< i << std::endl;
			
			if(next>0)
			{
				std::istringstream buffer(str.substr(i,next - i));
				buffer >> chord_note;
				//	std::cout << "debug in assign chord note : " << chord_note << std::endl;
			}
			
			//		if ((XMIN < chord_note) && (chord_note < XMAX))
			result->chord[j++] = chord_note;
		}
		result->size = chord_size;		
	}
	
	void init_start(void)
	{
		assign_arg_string(_start.value(),&startA1);
	}
	
	void init_end(void)
	{
		assign_arg_string(_end.value(),&endA1);
	}
	void init_domain(void)
	{
		assign_arg_string(_domain.value(),&chordA1);
	}
	
	void init_allowed_intervals(void)
	{
		assign_arg_string(_forbiddenI.value(),&forbiddenA1);
		for (int i = 0;i < forbiddenA1.size - 1 ;i++)
		{
			forbidden_itv_vector.push_back(forbiddenA1.chord[i]);
		}	
	}
	
};


class InterpolM : public Script {
protected:
	/// matrix dimensions
	int n;
	int steps;
	/// Fields of square
	IntVarArray x;
   
	
public:
	/// model variants
	enum {
		MODEL_1, // interpolation "par etats"
		MODEL_2 // interpolation "progressive"
	};
	/// Post constraints
	InterpolM(const interpolMOptions& opt):
	Script(opt),
    n(opt.n()),
	steps(opt.steps()),
	x(*this,n*steps,1,MAXPITCH)
	{
			
		// Matrix-wrapper for the square
		Matrix<IntVarArray> m(x, n, steps);
		BoolVar b1,b2;
		IntVarArgs d((n * (steps - 2))- 1);
        Rnd r(1U);
		
		// contraint 1 : fix debut et fin :
		
		int j = n*steps - n;
		for (int i = 0; i<n; i++) {
			rel(*this, x[i],IRT_EQ,startA1.chord[i]);
		}
		
		for (int i = 0; i<n; i++) {
			rel(*this, x[j+i],IRT_EQ,endA1.chord[i]);
		}
		
		if (opt.model() == MODEL_1)  // chaque note au rang j correspond à la note de rang j dans l'état initial ou final
		{
			
			//contrainte 2
			for (int i = 1; i<steps - 1; i++) 
				for (int j = 0; j<n; j++) 
				{	
					rel(*this,(x[(i * n) + j] == x[j]) || (x[(i * n) + j] == x[j + ((steps - 1) * n)]));
				}
			//contrainte 2b
			for (int i = 1; i<steps - 1; i++) 
				for (int j = 0; j<n; j++) 
				{	
					rel(*this,(x[(i * n) + j] == x[((i - 1) * n) + j] )||(x[(i * n) + j] == x[((i + 1) * n) + j]));
				}
		}
		else
		{
			//contrainte 2c  // interpolation progressive
			for (int i = 1; i<steps - 1; i++) 
				for (int j = 0; j<n; j++) 
				{	
						rel(*this,
									  ((x[j] < x[j + ((steps - 1) * n)]) && (x[(i * n) + j] >= x[((i - 1) * n) + j] ) && (x[(i * n) + j] <= x[((i + 1) * n) + j]))
									  || 
									  ((x[j] >= x[j + ((steps - 1) * n)]) && (x[(i * n) + j] <= x[((i - 1) * n) + j] ) && (x[(i * n) + j] >= x[((i + 1) * n) + j])));
				}
		}
		
		// contrainte  3 : Set up variables for intervals, 
		for (int i=0; i<(n * (steps -2))- 1; i++)
			//d[i] = abs(*this, Minus(*this,x[i+1+n],x[i+n],opt.icl()),opt.icl());
			d[i] = expr (*this,abs(x[i+1+n] - x[i+n]),opt.icl());

		//Constrain 3a : them to be in the allowed interval list		
		/*
		 int a[]={ 0,1,2,3,4,5,6,7,8,9,10,11};
		 IntSet allowed(a,12); 
		 dom(*this, d, allowed); 
		 */
		
		//Constrain 3b : intervalles interdits	
			for (int i=0; i < opt.forbidden_itv_vector.size(); i++) {
				count(*this,d,opt.forbidden_itv_vector[i],IRT_EQ,0);
			}
		
		//contrainte 4 ; note autorisées
		IntArgs chordArgs (chordA1.size,chordA1.chord);
		IntSet chord(chordArgs); //les notes de x appartiennent à l'accord

		dom(*this, x, chord); 
		
		if(opt.branching() == 0)
			branch(*this, x, INT_VAR_SIZE_MIN(), INT_VAL_SPLIT_MIN());
		else 
			branch(*this, x, INT_VAR_RND(r), INT_VAL_RND(r));
	}
	
	/// Constructor for cloning \a s
	InterpolM(bool share, InterpolM& s) 
	: Script(share,s), 
	n(s.n),
	steps(s.steps){
		x.update(*this, share, s.x);
	}
	
	/// Copy during cloning
	virtual Space*
	copy(bool share) {
		return new InterpolM(share,*this);
	}
	/// Print solution
	virtual void
	print(std::ostream& os) const {
		// Matrix-wrapper for the square
		Matrix<IntVarArray> m(x, n, steps);
		
		for (int j = 0; j < steps; j++) {
			os << "\t";
			for (int i = 0; i<n; i++) {
				os.width(2);
				os << m(i,j) << " ";
			}
			os << std::endl;
		}
		
		outputstring << "(";
		for (int i = 0; i<n*steps; i++) {
			outputstring << x[i] << " ";
		}
		outputstring << ")";
		
		os << std::endl;
		
	}
	
};

/** \brief Main-function
 *  \relates MagicSquare
 */
int
main(int argc, char* argv[]) {
	interpolMOptions opt("InterpolM","(1 2 3)",3,3);
	opt.iterations(1);

	
	opt.model(InterpolM::MODEL_1);
	opt.model(InterpolM::MODEL_1, "simple", "modele de base");
	opt.model(InterpolM::MODEL_2, "progressif", "modele progressif");
	
	opt.branching(0); //normal
	opt.branching(0, "normal", "normal  branching");
	opt.branching(1, "random", "random  branching");

	opt.parse(argc,argv);

	opt.init_start();
	opt.init_end();
	opt.init_domain();

	opt.init_allowed_intervals();
		
	Script::run<InterpolM,DFS,interpolMOptions>(opt);
	return 0;
}


/* function to be called in the framework version */


extern "C" {
	char *interpolmG(char *str1,char *str2, char *str3, char *str4,int steps,int n,int n_solutions, int timelimit, int branching, char *resultat)
	{ 
		interpolMOptions opt("InterpolM",str1,steps,n);
		std::string chaine;
		
		opt.iterations(5);
		opt.model(InterpolM::MODEL_1);
		opt.branching(branching);
		opt.solutions(n_solutions);		 
		opt.time(timelimit);
		opt._start.value(str1);
		opt._end.value(str2);
		opt._domain.value(str3);
		opt._forbiddenI.value(str4);
		
		//	opt.size(n);
		//	opt._intervals.append(str1);
		//	opt.xfirst = a;
		//	opt.xlast = b;
		
		/*	opt.modelx(model);
		 if (opt.size() < 2) {
		 std::cerr << "size must be at least 2!" << std::endl;
		 return NULL;
		 }	
		 */
		
		opt.init_start();
		opt.init_end();
		opt.init_domain();
		opt.init_allowed_intervals();

		strcpy(result,"nil");
		outputstring.str("x");
		outputstring << "(" ;
		Script::run<InterpolM,DFS,interpolMOptions>(opt);
		
		outputstring << ")" ;
		
		chaine = outputstring.str();
		
		// pour éviter de déborder la chaine de sortie:
		if (chaine.size() > 8192) {
			std::cerr << "result string is too long - ";
			chaine.resize(8192);
		}

		strcpy(result,chaine.c_str());		
		
		std::cerr << "r1 " << result << std::endl;
		
		sprintf(resultat, "%s", result);
		
		std::cerr << "r2 " << resultat << std::endl;
		
		return result;
	}
	
};

int mainTOTEST(int argc, char* argv[]) //_for_testing__the_framework
{	
	char r[10000];
//	interpolmG("(2 2 3)","(4 5 2)","(1 2 3 4 5 6)","(0 2)",3,3,0,0,r);
	
	interpolmG("(1 2 3)","(5 7 9)","(1 2 3 4 5 6 7 8 9)","(0 1 6)",12,3,0,0,0,r);
	std::cerr << "result string is : " << r << std::endl;
	return 1;
}

/*
 ./interpolm -start "( 68.5 60. 63. 72. 66. 71. 66. )" -end "( 68.5 60. 63. 72. 66. 71. 66. )" -domain "( 68.5 60. 63. 72. 66. 71. 66. )" -n 7 -steps 12
*/
