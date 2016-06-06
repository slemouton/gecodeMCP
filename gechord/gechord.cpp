/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *   Serge Lemouton ircam 2009
 *
 */

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/set.hh>

#include <iostream>
#include <fstream>


#include "gechord.h"

using namespace Gecode;

#define XMIN 21
#define XMAX 120
void string_overflow_protect();

class Cell {
public:
	int n;
	//	std::vector <int> cell;
	int size;
	int cell[3];
};
class Chord {
public:
	int size;
	int octaviation; // 0/1 
	int chord[48];
};


namespace  {
	
//	Chord chordA1b={7,0,{60, 65, 66, 68, 71, 76, 82}}; // modified by arguments
	Cell cellsAb[] = 
	{
	{1,2,{-3, -8}},{1,2,{3, 8}},
	{1,2,{6, -6}},{1,2,{-6, 6}},
	{1,2,{11, -11}},{1,2,{-11, 11}},
	{1,2,{-3, 11}},
	};
	
	
}

/* 
 * options 
 */
class GechordOptions : public SizeOptions {
private:
	Driver::UnsignedIntOption _modelx;
	
public:
	std::string _intervals ;//< Parameters to be given on command line
	//	std::string _chord ;
	unsigned int xfirst;
	unsigned int xlast;
	unsigned int intervals_n;	
	
	/// Initialize options for example with name \a s
	GechordOptions(const char* s,
				   const char* _cells, unsigned int xfirst,unsigned int xlast,unsigned int model0)
	: SizeOptions(s)
	, _modelx("-modelx","0 : considere les intervalles successifs; 1 : tous les intervalles",model0){
		add(_modelx);
	}
	/// Return model
	unsigned int modelx(void) const { return _modelx.value(); }
	/// set model
	void modelx(unsigned int v)  { _modelx.value(v) ; }

	/// Parse options from arguments \a argv (number is \a argc)
	void parse(int& argc, char* argv[]) {
		//		if (argc < 4) 
		//			return;
		if (!strcmp(argv[1],"-help") || !strcmp(argv[1],"--help") || !strcmp(argv[1],"-?"))
		{
			help();
			exit(EXIT_SUCCESS);
		}
		
		
		std::cout << "-" << argv[1] << "-" << argv[2]<< "-" << argv[3]<< std::endl;
		
		size(atoi(argv[1]));
		_intervals.append(argv[2]);
		xfirst = atoi(argv[3]);
		xlast = atoi(argv[4]);
		
		// Remove my 3 arguments (size, intervals, first and last note)
		argc -= 4;
		for (int i=1; i<argc; i++)
			argv[i] = argv[i+4];		
		
		// parse other options !!!!!!!!
		Options::parse(argc,argv);
		
		derive();
		
	}
	/// Print help message
	virtual void help(void) {
		Options::help();
		std::cerr << "\t(unsigned int) default: " << size() << std::endl
		<< "\t\tparameter size" << std::endl		
		<< "\t(string) default: " << _intervals << std::endl
		<< "\t\tparameter cells" << std::endl
		<< "\t(unsigned int) default: " << xfirst << std::endl
		<< "\t\tparameter first_note" << std::endl
		<< "\t(unsigned int) default: " << xlast << std::endl
		<< "\t\tparameter last_note" << std::endl;
		
	}
	
	
	/// Derive cells and chords from parameter (lisp-like) strings
	void derive(void) {
		int k = 0;
		
		std::cout << "compute problem input data : " << _intervals << " - " << std::endl;
		
		// 2.parse and calculate cells and intervals :
		_intervals.erase(0,1);
				
			std::cout << "before processing : " << _intervals << std::endl;
		//erase extra spaces
		for(int i = _intervals.find(" ", 0); i != -1; i = _intervals.find(" ", i))
		{
			if ((_intervals[i-1] == ' ') or (_intervals[i-1] == '(') or (_intervals[i-1] == ')')or (_intervals[i+1] == ')'))  //supprime les espaces en trop
				_intervals.erase(i,1);
			else
				i++;  
		}
			std::cout << "after processing : " << _intervals << std::endl;
		

		
		for(int i = _intervals.find("(", 0); i != std::string::npos; i = _intervals.find("(", i))
		{
			int next = 0,num = 0,sub_next = 0,cell_int,kk=0,fin = 0;
			std::string sous_chaine;
			next = _intervals.find(")", i);
			sous_chaine = _intervals.substr(i,next - i);
			i = next;
			
			sub_next = sous_chaine.find(" ", 1);
			std::istringstream buffer(sous_chaine.substr(1,sub_next - 1));
			buffer >> num;
			cellsAb[k].n = num;
			
			for(int ii = sub_next+1 ; ii != std::string::npos; ii = sous_chaine.find(" ", ii))
			{
				sub_next = sous_chaine.find(" ", ii);
				if (ii == sub_next)
					fin = std::string::npos;
				else
					fin = sub_next - ii;
				std::istringstream buffer(sous_chaine.substr(ii,fin));
				buffer >> cell_int;
				cellsAb[k].cell[kk++] = cell_int;
				cellsAb[k].size = kk;
				ii++;
			}
			intervals_n = k;
			k++;	
		}
		//DEBUGGING 
#if 0
		std::cout << "intervals_n " << intervals_n << std::endl;
		
		for (int i=0;i<=intervals_n ;i++)
		{
			std::cout << cellsAb[i].size << " : " << cellsAb[i].cell[0] << " " << cellsAb[i].cell[1]<< std::endl;
		}
		
		// 3.liste des intervalles :
		//			for (std::list<int>::iterator it=itv_list.begin(); it!=itv_list.end(); ++it)
		//				std::cout << " " << *it;
		//			std::cout << "  <- 1" << std::endl;
#endif
		///////////////
	}
	
	
	
};

/*
 * 
 *
 *generer un accord avec les contraintes suivantes : les notes appartiennent à un domaine donné, les intervalles successifs à une liste de cellules. peut-on le faire en modelisant l’accord par un SetVar (ensemble gecode) ?
 *
 */

class Gechord : public Script {
private:
	/// The chord
	IntVarArray chord;
	int intervals_n;
	
public:
	/// model variants
	enum {
		MODEL_BASIC,
		MODEL_ALL // oblige tous les intervalles à faire partie des cellules
	};
	/// Actual model
	Gechord(const GechordOptions& opt) :
	Script(opt),
    chord(*this,opt.size(),XMIN,XMAX) {
		const int n = chord.size();
		int k =0;
		IntVarArgs d(n-1);  // intervalles entre chaque note de l'accord-solution
		IntVarArgs dAll(n*(n-1)/2.);  // tous les intervalles entre chaque note deux à deux de l'accord-solution
		intervals_n = opt.intervals_n;
		
		// Set up variables for distance (intervals)
		if (opt.modelx() == MODEL_BASIC)
			for (int i=0; i<n-1; i++)
				d[i] =  expr(*this,chord[i+1] - chord[i],opt.icl());
		
		else 
			for (int i=0; i<n-1; i++)
				for(unsigned int j = i + 1; j < n;j ++)
					dAll[k++] = expr(*this, chord[j] - chord[i], opt.icl());
		
		/* constraints */
		
		/*contrainte 0. domaine: les notes appartiennent à un accord donné */
#if 0
		IntArgs chordArgs (chordA1b.size,chordA1b.chord);
		IntSet input_chord(chordArgs); 
		IntVarArray c;
		//		IntVar input_chord(*this,chordArgs);
		dom(*this,chord,input_chord);	
#endif
		
		//contrainte 1 : début et fin
		if(opt.xfirst !=0)
			rel(*this,chord[0],IRT_EQ,opt.xfirst);
		if(opt.xlast !=0)
			rel(*this,chord[n-1],IRT_EQ,opt.xlast);
		
		//contrainte 2 : ordre ascendant
		rel(*this, chord, IRT_LE);
		
		//contrainte 3 : intervalles
		if (opt.modelx() == MODEL_BASIC)
		{
			for (int i=0;i<=intervals_n ;i++)
				if (cellsAb[i].n == 0) // forbidden intervals
					count(*this,d,cellsAb[i].cell[0],IRT_EQ,cellsAb[i].n);
			else
				count(*this,d,cellsAb[i].cell[0],IRT_GQ,cellsAb[i].n);
		}
		else
		{
			for (int i=0;i<=intervals_n ;i++)
				if (cellsAb[i].n == 0) // forbidden intervals
					count(*this,dAll,cellsAb[i].cell[0],IRT_EQ,cellsAb[i].n);
				else
					count(*this,dAll,cellsAb[i].cell[0],IRT_GQ,cellsAb[i].n);
		}
		branch(*this, chord, INT_VAR_SIZE_MIN(), INT_VAL_SPLIT_MIN());
	}
	
	/// Print solution
	virtual void
	print(std::ostream& os) const {
	//	std::ostringstream outputstring1;
		outputstring << "(" ;
		
		for (int i=0; i<chord.size(); i++) {
			os << " " << chord[i];
			outputstring << " " << chord[i];
		}
		os  << std::endl;
		outputstring << ")" ;
	//	strcpy(result,outputstring1.str().c_str());		

	}
	
	/// Constructor for copying \a s
	Gechord(bool share, Gechord& s) : Script(share,s) {
		chord.update(*this, share, s.chord);
	}
	/// Copy during cloning
	virtual Space*
	copy(bool share) {
		return new Gechord(share,*this);
	}
	
};

/** \brief Main-function
 *  \relates Hamming
 */
int main(int argc, char* argv[]) {
  GechordOptions opt("Gechord","((1 3)(2 4))",0,0,0);
  opt.parse(argc,argv);
  Script::run<Gechord,DFS,GechordOptions>(opt);
  return 0;
}

/* function to be called in the framework version */


extern "C" {
	char *gechordG(int n,char *str1, int a, int b,int n_solutions, int timelimit, unsigned int model, char *resultat)
	{ 
		GechordOptions opt("Gechord","((1 3)(2 4))",0,0,0);
		
		opt.iterations(5);
		opt.solutions(n_solutions);		 
		opt.time(timelimit);
		
		opt.size(n);
		opt._intervals.append(str1);
		opt.xfirst = a;
		opt.xlast = b;
		
		opt.modelx(model);
		//	opt.model(Gechord::MODEL_BASIC, "simple", "modele de base");
		//	opt.model(Gechord::MODEL_ALL, "redondant", "considere tous les intervalles presents dans l'accord");		
		
		if (opt.size() < 2) {
			std::cerr << "size must be at least 2!" << std::endl;
			return NULL;
		}	
		
		opt.derive();
		strcpy(result,"nil");
		outputstring.str("x");
		outputstring << "(" ;
		//		outputstring << "(" << opt.model ;
		Script::run<Gechord,DFS,GechordOptions>(opt);
		
		outputstring << ")" ;
		strcpy(result,outputstring.str().c_str());	
		
		string_overflow_protect();
		
		sprintf(resultat, "%s", result);
		
		std::cerr << "model" << opt.modelx() <<" result: " << resultat << std::endl;
		
		return result;
	}
	
};
void string_overflow_protect()
// pour éviter de déborder la chaine de sortie:
{
	std::string chaine;
	chaine = outputstring.str();
	std::cerr << "chaine size = "<<chaine.size()<< std::endl;
	
	if (chaine.size() > MAXSTRINGLENGTH) {
		std::cerr << "result string is too long - ";
		chaine.resize(MAXSTRINGLENGTH);
	}
	
	strcpy(result,chaine.c_str());		
}


int maintotest(int argc, char* argv[]){
	
	char r[MAXSTRINGLENGTH];
//	gechordG(5, "((1 2)(2 3) (1 5))",61,0,0,0,1,r);
	gechordG(5,  "((2 1)(1 2)(0 3)(1 6)(0 10))",60,0,12,0,1,r);
	
	std::cerr << "result string is " << r << std::endl;
	return 1;
}

/* example
gechord 5 "((2 1)(1 2)(0 3)(1 6)(0 10))" 60 0 -modelx 1 -solutions 12
*/

