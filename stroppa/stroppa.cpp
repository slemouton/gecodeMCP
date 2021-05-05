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
#include <map>


#include "stroppa.h"

using namespace Gecode;

#define XMIN 21
#define XMAX 120

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

//namespace  {

static	Chord chordA1={7,0,{60, 65, 66, 68, 71, 76, 82}}; // modified by arguments

//}


/* 
 * options 
 */
class StroppaOptions : public Options {
private:
	Driver::UnsignedIntOption _n; //nombres de notes	
	Driver::StringValueOption _cs; // limites min et max du coef de stability
	Driver::StringValueOption _domain; // notes autorisées
	Driver::StringValueOption _density; // limites min et max de density
	
public:
	std::string _intervals ;//< Parameters to be given on command line
	unsigned int xfirst;
	unsigned int xlast;
	float densmin;	
	float densmax;
	float csmin;
	float csmax;
	std::map<int,int> weight_space;
	int ws[99];
	
	/// Initialize options for example with name \a s
	StroppaOptions(const char* s,int n0=5)
	: Options(s),
	_n("-n","note number",n0),
	_density("-density", "limites min et max de density","(0. 0.)"),
	_cs("-cs", "limites min et max de stability","(0. 0.)"),
	_domain("-domain", "domain","(60 61 62)")
	
	{
		// add options

		add(_n);add(_density);add(_cs);
	}
	/// Return model
	unsigned int size(void) const { return _n.value(); }
	void size(unsigned int n)  {  _n.value(n); }
	void density(char *str)  {  _density.value(str); }
	void cs(char *str)  {  _cs.value(str); }
	void domain(char *str)  {  _domain.value(str); }

	/// set model

	void assign_limit_string(std::string str,float *min,float *max)
	{	int first_elem = 0;
		float val;
		
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
			i++;  // Move past the last discovered instance to avoid finding same
			next = str.find(" ", i);
			if(next>0)
			{
				std::istringstream buffer(str.substr(i,next - i));
				buffer >> val;
			}
			
			if(!first_elem)
			{
				*min = val;
				first_elem = 1;
			}
			*max = val;

		}

	}
		
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
	
	void init_density(void)
	{
		assign_limit_string(_density.value(),&densmin,&densmax);
		if (densmin < 0.001)
			densmin = 0.001;
	}
	
	void init_cs(void)
	{
		assign_limit_string(_cs.value(),&csmin,&csmax);
	}
	void init_domain(void)
	{
		assign_arg_string(_domain.value(),&chordA1);
	}
	void init_wspace(void)
	{
		//float itvl_weight[] = {0.0, 10.0, 7.5, 1.5, 1.0, 3.5, 4.0, 2.0, 2.5, 3.0, 7.0, 9.5};
		//float octave_weight[] = {1.0, 0.85, 0.7, 0.45, 0.25, 0.1, 0.0};
		int itvl_weight[] = {0, 100, 75, 15, 10, 35, 40, 20, 25, 30, 70, 95};
		int octave_weight[] = {100, 85, 70, 45, 25, 10, 0, 0};

		for (int i = 0; i<84; i++) {
			int oct = i/12;
			int itv = i%12;
			weight_space[i] = itvl_weight[itv] * octave_weight[oct];
						ws[i] = itvl_weight[itv] * octave_weight[oct];
//			std::cout << "Weigth [" << i << "]= " << weight_space[i]<<std::endl;
		}	
	}
	
};

/*
 * 
 *
 *generer un accord avec les contraintes suivantes : les notes appartiennent à un domaine donné, peut-on le faire en modelisant l’accord par un SetVar (ensemble gecode) ?
 *
 */

class Stroppa : public Script {
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
	Stroppa(const StroppaOptions& opt) :
	Script(opt),
    chord(*this,opt.size(),XMIN,XMAX) {
		const int n = chord.size();
		int k = 0;
		IntVar surface;
		IntVarArgs d(n-1);  // intervalles entre chaque note de l'accord-solution
		IntVarArgs dAll(n*(n-1)/2.);  // tous les intervalles entre chaque note deux à deux de l'accord-solution
		IntVarArgs poids(n-1);  // poids pour chaque intervalle
		IntVar cs(*this,0,100000);
		IntArgs e_{12, 0,1,2,3,4,5,6,7,8,9,10,11};
		//IntArgs w_(12, 0, 100, 75, 15, 10, 35, 40, 20, 25, 30, 70, 95);
		//IntArgs w_(12, 0.0, 1000.0, 750.0, 150.0, 100.0, 350.0, 400.0, 200.0, 250.0, 300.0, 700.0, 950.0);
		IntArgs w_{128, 
				   0, 1000, 750, 150, 100, 350, 400, 200, 250, 300, 700, 950, 
		 0, 850, 638, 128, 85, 298, 340, 170, 212, 255, 595, 808, 
		 0, 700, 525, 105, 70, 245, 280, 140, 175, 210, 490, 665, 
		 0, 450, 338, 67, 45, 158, 180, 90, 112, 135, 315, 428, 
		 0, 250, 188, 38, 25, 88, 100, 50, 62, 75, 175, 238, 
		 0, 100, 75, 15, 10, 35, 40, 20, 25, 30, 70, 95, 
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		 0, 0, 0, 0, 0, 0, 0, 0};
		IntVarArray c_(*this,128,0,100); //compteur d'intervalles
		
		
		// Set up variables for distance (intervals)
		
		for (int i=0; i<n-1; i++)
			d[i] =  expr(*this,chord[i+1] - chord[i],opt.ipl());

		for (int i=0; i<n-1; i++)
			for(unsigned int j = i + 1; j < n;j ++)
				dAll[k++] = expr(*this, chord[j] - chord[i], opt.ipl());		
		
		surface = expr(*this,(chord[n-1] - chord[0]));
		
		/* constraints */
		
		/*contrainte 0. domaine: les notes appartiennent à un accord donné */
		
		// 0b : nombre de note compris entre nnmin et nnmax
		
		
		//contrainte 1 : début et fin
		if(opt.xfirst !=0)
			rel(*this,chord[0],IRT_EQ,opt.xfirst);
		if(opt.xlast !=0)
			rel(*this,chord[n-1],IRT_EQ,opt.xlast);
		
		//contrainte 1b : surface (intervalle entre les voix extremes comprise entre surfmin et surfmax
		
		//contrainte 1c : densite (nn/(surf + 1)) entre densmin et densmax
		if(opt.densmax !=0)
		{
			rel(*this,surface,IRT_LQ, floor(opt.size()/opt.densmin - 1.));
			rel(*this,surface,IRT_GQ, ceil(opt.size()/opt.densmax - 1.));
		}
		
		
		//contrainte 2 : ordre ascendant
		rel(*this, chord, IRT_LE);
		
		//contrainte 3 : contient la (les) notes données
		
		//contrainte 4 : coefficient de stabilité entre csmin et csmax
		if (opt.csmax != 0)
		{
	//		count(*this, d, c_,opt.ipl());
			count(*this, dAll, c_,opt.ipl());
			linear(*this, w_, c_ , IRT_EQ, cs,IPL_DEF);
			rel(*this,cs,IRT_GQ,opt.csmin * 100. * n*(n-1)/2.);
			rel(*this,cs,IRT_LQ,opt.csmax * 100. * n*(n-1)/2.);
		}

		//branching
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

	}
	
	/// Constructor for copying \a s
	Stroppa(Stroppa& s) : Script(s) {
		chord.update(*this, s.chord);
	}
	/// Copy during cloning
	virtual Space*
	copy(void) {
		return new Stroppa(*this);
	}
};

/** \brief Main-function
 *  \relates Hamming
 */
#if SCRIPT
int main(int argc, char* argv[]){
#else
int main_stroppa (int argc, char* argv[]){
#endif
  StroppaOptions opt("Stroppa",5);
  opt.parse(argc,argv);
	
	opt.init_density();
	opt.init_cs();
	opt.init_domain();
	opt.init_wspace();
	opt.xfirst = 0;
	opt.xlast = 0;

	std::cerr << "density between " << opt.densmin << " et " << opt.densmax <<std::endl;	
	
  Script::run<Stroppa,DFS,StroppaOptions>(opt);
  return 0;
}

/* function to be called in the framework version */


extern "C" {
	char *gVpsG(int n, int a,int b,char *density,char *cs,char *domain,int n_solutions, int model,int timelimit, char *resultat)
	{ 
		StroppaOptions opt("Stroppa",5);
		
		opt.size(n);
		opt.xfirst = a;
		opt.xlast = b;
		opt.density(density);
		opt.cs(cs);
		opt.domain(domain);
		
		opt.solutions(n_solutions);		 
		opt.time(timelimit);
	
		opt.init_density();
		opt.init_cs();
		opt.init_domain();
		opt.init_wspace();
		
		opt.iterations(5);
		
		if (opt.size() < 2) {
			std::cerr << "size must be at least 2!" << std::endl;
			return NULL;
		}	
		
		strcpy(result,"nil");
		outputstring.str("x");
		outputstring << "(" ;
		Script::run<Stroppa,DFS,StroppaOptions>(opt);
		
		outputstring << ")" ;
		strcpy(result,outputstring.str().c_str());		
		
		std::cerr << "r1 " << result << std::endl;
	
		sprintf(resultat, "%s", result);

		std::cerr << "r2 " << resultat << std::endl;
		
		return result;
	}
	
};



int main__(int argc, char* argv[]){
	
	char r[10000];
//	gVpsG(4,61,0,"(0 0)","(0 0)","(0)",4,0,r);
	
	std::cerr << "result string is " << r << std::endl;
	return 1;
}

