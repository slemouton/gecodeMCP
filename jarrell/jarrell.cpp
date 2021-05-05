/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main author:
 *  serge
 * see the examples at the bottom of this file !
 *
 * TODO
 * mise en forme des chaines d'entrées (enlever les espaces en trop) : regexp to preprocess lisp-like list
 * play with icl
 * voir si l'on peut trouver directement la meilleur solution avec search "bab" (au lieu de "BAB") 
 * voir si l'on gagne en restreignant le domaine des n-intervalles (entre ITV_MIN et ITV_MAX)
 */

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

#include <cstdlib>
#include <list>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace Gecode;

#define XMIN 21
#define XMAX 120

#define ITV_MIN -16
#define ITV_MAX 16

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
	// example A, extrait de Congruences, cf cahiers IRCAM, p.51
	const Cell cellsA[] = 
	{
	{1,2,{-3, -8}},{1,2,{3, 8}},
	{1,2,{6, -6}},{1,2,{-6, 6}},
	{1,2,{11, -11}},{1,2,{-11, 11}},
	{1,2,{-3, 11}},
	};
	const Chord chordA1={7,0,{60, 65, 66, 68, 71, 76, 82}};
	
	Chord chordA1b={7,0,{60, 65, 66, 68, 71, 76, 82}}; // modified by arguments
	Cell cellsAb[] = 
	{
	{1,2,{-3, -8}},{1,2,{3, 8}},
	{1,2,{6, -6}},{1,2,{-6, 6}},
	{1,2,{11, -11}},{1,2,{-11, 11}},
	{1,2,{-3, 11}},
	};

	const Chord chordA2={10,0,{59, 60, 62, 63, 65, 66, 70, 71, 73, 76}};
	
	// example B, cf article Courtot
	const Cell cellsB[] =
	{
	{2,3, {-11, 13, 1}},
	{4,2, {13, -11}},
	{2,3, {16, -14, 1}}
	};
	
	// example C
	const Cell cellsC[] = 
	{
	{1,2,{-3 -8}},{1,2,{-3, -8}},
	{1,1,{6}},{1,1,{-6}},
	{1,1,{11}},{1,1,{-11}},
	};
	const Chord chordC={5,1,{60, 63, 65, 66, 71}};
	
}

int cell2encode(int x1,int x2)	{return 1000*(x1+100)+(x2+100);}
int cell3encode(int x1,int x2,int x3)	{return 1000000*(x1+100)+1000*(x2+100)+(x3+100);}

static char result[] = "le resultat est                                                                         ";
std::ofstream output_file;

class JarrellOptions : public SizeOptions {
public:
	std::string _cells ;//< Parameters to be given on command line
	std::string _chord ;
	std::vector<int> itv_vector; // to keep a track of all the required interval
	std::vector<int> forbidden_itv_vector; // to keep a track of all the forbidden interval

	unsigned int xfirst;
	unsigned int xlast;
	unsigned int cell_n;	
	
	/// Derive cells and chords from parameter (lisp-like) strings
	void derive(void) {
		int j = 0,k = 0, chord_note, chord_size = 0;
		std::list<int> itv_list; // to keep a track of all the required interval
		std::list<int> forbidden_itv_list; // to keep a track of all the required interval
		
		std::cout << "compute problem input data : " << _cells << " - " << _chord << " [" << _chord.length()<< "] " <<std::endl;
		
		// 1.normalisation de la chaine d'entrée (en enlevant simplement les espaces en trop !!!!)
		// + parse and calculate chord :
		
		for(int i = _chord.find("(", 0); i != -1; i = _chord.find("(", i))
		{
			_chord.replace(i,1," ");
			i++;  // Move past the last discovered instance to avoid finding same
		}
		for(int i = _chord.find(")", 0); i != -1; i = _chord.find(")", i))
		{
			_chord.replace(i,1," ");
			i++;  // Move past the last discovered instance to avoid finding same
		}

		for(int i = _chord.find(" ", 0); i != -1; i = _chord.find(" ", i))
		{
			if (_chord[i-1] == ' ')  //supprime les espaces en trop
			{
				_chord.erase(i,1);
				i--;
			} 
			i++;  
		}

		for(int i = _chord.find(" ", 0); i != std::string::npos; i = _chord.find(" ", i))
		{
			int next = 0;
			chord_size++;
			i++;  // Move past the last discovered instance to avoid finding same
			next = _chord.find(" ", i);
		//	std::cout << "next,i"  << next << ","<< i << std::endl;

			if(next>0)
			{
				std::istringstream buffer(_chord.substr(i,next - i));
				buffer >> chord_note;
	//			std::cout << "chord note : " << chord_note << std::endl;
			}
		
			if ((XMIN < chord_note) && (chord_note < XMAX))
				chordA1b.chord[j++] = chord_note;
		}
		chordA1b.size = chord_size;
		
		// 2.parse and calculate cells and intervals :
		_cells.erase(0,1);
		
	//		std::cout << "before processing : " << _cells << std::endl;
		//erase extra spaces
		for(int i = _cells.find(" ", 0); i != -1; i = _cells.find(" ", i))
		{
			if ((_cells[i-1] == ' ') or (_cells[i-1] == '(') or (_cells[i-1] == ')')or (_cells[i+1] == ')'))  //supprime les espaces en trop
				_cells.erase(i,1);
			else
			i++;  
		}
	//	std::cout << "after processing : " << _cells << std::endl;
		if (!_chord.length()) {
			std::cout << "empty chord " << _chord << std::endl;
			exit(EXIT_FAILURE);
		}
		if ((!_cells.length())||(chord_size == 0)) {
	std::cout << "empty cells " << _cells << std::endl;
	exit(EXIT_FAILURE);
}
		for(int i = _cells.find("(", 0); i != std::string::npos; i = _cells.find("(", i))
		{
			int next = 0,num = 0,sub_next = 0,cell_int,kk=0,fin = 0;
			std::string sous_chaine;
			next = _cells.find("))", i);
			sous_chaine = _cells.substr(i,next - i);
			i = next;
			
			sub_next = sous_chaine.find("(", 1);
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
				
				if (num == 0)
					forbidden_itv_list.push_back(cell_int);
				else					
					itv_list.push_back(cell_int);

				ii++;
			}
			cell_n = k;
			k++;	
		}
		//DEBUGGING 
#if 0
		std::cout << "cell_n " << cell_n << std::endl;
		
		for (int i=0;i<=cell_n ;i++)
		{
			std::cout << cellsAb[i].size << " : " << cellsAb[i].cell[0] << " " << cellsAb[i].cell[1]<< std::endl;
		}
		
		// 3.liste des intervalles :
		for (std::list<int>::iterator it=itv_list.begin(); it!=itv_list.end(); ++it)
			std::cout << " " << *it;
		std::cout << "  <- 1" << std::endl;
#endif
		///////////////
		// 3.liste des intervalles :
		itv_list.sort();
		itv_list.unique();
		for (std::list<int>::iterator it=itv_list.begin(); it!=itv_list.end(); ++it)
		{
			itv_vector.push_back(*it);
		}
		
		for (std::list<int>::iterator it=forbidden_itv_list.begin(); it!=forbidden_itv_list.end(); ++it)
		{
			forbidden_itv_vector.push_back(*it);
		}
	}
	
	/// Initialize options for example with name \a s
	JarrellOptions(const char* s,
				   const char* _cells, const char* _chord, int xfirst,int xlast)
    : SizeOptions(s) {
	}
	/// Parse options from arguments \a argv (number is \a argc)
	void parse(int& argc, char* argv[]) {
		//	
		if (!strcmp(argv[1],"-help") || !strcmp(argv[1],"--help") || !strcmp(argv[1],"-?"))
		{
			help();
			exit(EXIT_SUCCESS);
		}
		if (argc < 5) 
		{
			std::cerr << "jarrell needs 5 args" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		//		std::cout << "-" << argv[1] << "-" << argv[2]<< "-" << argv[3]<< std::endl;
		
		size(atoi(argv[1]));
		_cells.append(argv[2]);
		_chord.append(argv[3]);
		xfirst = atoi(argv[4]);
		xlast = atoi(argv[5]);
		
		// Remove my 3 arguments (size, cells, chord, first and last note)
		argc -= 5;
		for (int i=1; i<argc; i++)
			argv[i] = argv[i+5];
		
		
		// parse other options !!!!!!!!
		Options::parse(argc,argv);
		
		derive();
		
	}
	/// Print help message
	virtual void help(void) {
		Options::help();
		std::cerr << "\t(unsigned int) default: " << size() << std::endl
		<< "\t\tparameter size" << std::endl
		
		<< "\t(string) default: " << _cells << std::endl
		<< "\t\tparameter cells" << std::endl
		
		<< "\t(string) default: " << _chord << std::endl
		<< "\t\tparameter chord" << std::endl
		
		<< "\t(unsigned int) default: " << xfirst << std::endl
		<< "\t\tparameter first_note" << std::endl
		<< "\t(unsigned int) default: " << xlast << std::endl
		<< "\t\tparameter last_note" << std::endl;
		
	}
};

class Jarrell : public IntMaximizeScript {
private:
  /// The numbers
  IntVarArray x;
	/// How many cells are used in the melody ("cost", should be maximized)
	IntVar k;
	
	public:
	/// model variants
	enum {
		MODEL_SIMPLE,
		MODEL_ITV // oblige tous les intervalles à faire partie des cellules
	};
	/// search variants
	enum {SEARCH_DFS,   //use depth first search algorithm
		SEARCH_BAB,     // use branch and bound to optimize
		SEARCH_RESTART,  /// use restart to optimize 
		SEARCH_BEST  /// use restart to optimize 
	};
	int cell_n; //nombre de cellules mélodiques
	
  /// Actual model
  Jarrell(const JarrellOptions& opt ) :
  	IntMaximizeScript(opt),
    x(*this, opt.size(), XMIN, XMAX),k(*this,0,50) {
		const int n = x.size();
	//	const int cell_n  = 3; //nombre de cellules mélodiques
		
		//	IntArgs chordArgs(4,60,61,64,65);
		IntArgs chordArgs (chordA1b.size,chordA1b.chord);
		IntSet chord(chordArgs); //les notes de x appartiennent à l'accord
	//	const bool accord_octaviation = chordA1.octaviation; // repetition octaviante des notes de l'accord
		IntVarArgs d(n-1);  // intervalles entre chaque note de la solution, x
	//	IntVarArray cells_seq(*this,n-1,0,cell_n); // sequence de cellules
		IntVarArray intervals_cells(*this,n-1,0,11); // sequence de cellules sous forme d'intervalles (doit être == d)
		IntVarArray cells2(*this,n-2,0,200200); // sequence de cellules de longueur 2
		IntVarArray cells3(*this,n-3,0,200200200); // sequence de cellules de longueur 3
		IntVarArray costs(*this,opt.cell_n+1,0,24);
		
		cell_n = opt.cell_n;
		
		// Set up variables for distance (intervals)
		for (int i=0; i<n-1; i++)
			//d[i] =  minus(*this,x[i+1],x[i],opt.ipl());
			d[i] =  expr(*this,x[i+1]-x[i],opt.ipl());
		// Set up variables for 2-length cell
		for (int i=0; i<n-2; i++)
			cells2[i] = expr(*this,(d[i]+100)*1000+(d[i+1]+100));		
		// Set up variables for 3-length cell
			for (int i=0; i<n-3; i++)
				cells3[i] = expr(*this,(d[i]+100)*1000000+(d[i+1]+100)*1000+(d[i+2]+100));		
		
		//contrainte 1 : début et fin
		if(opt.xfirst !=0)
			rel(*this,x[0],IRT_EQ,opt.xfirst);
		if(opt.xlast !=0)
			rel(*this,x[n-1],IRT_EQ,opt.xlast);
		
		//contrainte 2 : toutes les notes appartiennent à l'accord
		dom(*this,x,chord);
		
		//contrainte 3 : la mélodie ne contient que les intervalles dans un domaine donné (contrainte redondante mais utile !)
		if (opt.model() == MODEL_ITV)
		{
			IntArgs a(opt.itv_vector.size(),&opt.itv_vector[0]);
			IntSet interval_domain(a);
			dom(*this,d,interval_domain);
		}
	
		else 
			//contrainte 4 : la mélodie ne contient pas les intervalles interdits (ceux qui ont pour quantité 0)
		{
			for (int i=0; i < opt.forbidden_itv_vector.size(); i++) {
				count(*this,d,opt.forbidden_itv_vector[i],IRT_EQ,0);
			}
		}

		
		//contrainte 5 (obsolete) : la mélodie contient au moins n fois un intervalle donné
		if(0)
		{
			int i_5 = 6;
			int n_5 = 1;
			count(*this,d,i_5,IRT_GQ,n_5);
		}
		
		//contrainte 6 (obsolete)  : la mélodie contient au moins n fois une cellule donnée
		if (0)
		{
			int c_6 = cell2encode(6,-6);
			int n_6 = 5;
			
			std::cout << c_6 << std::endl;
			count(*this,cells2,c_6,IRT_GQ,n_6);
		}
	
		//contrainte 7 : cas général : la mélodie contient un ensemble de cellules données avec des quantités données
		for (int i=0;i<=cell_n ;i++)
		{
			if (cellsAb[i].size == 1)
				count(*this,d,cellsAb[i].cell[0],IRT_GQ,cellsAb[i].n);
			if (cellsAb[i].size == 2)
				count(*this,cells2,cell2encode(cellsAb[i].cell[0],cellsAb[i].cell[1]),IRT_GQ,cellsAb[i].n);
			if (cellsAb[i].size == 3)
				count(*this,cells3,cell3encode(cellsAb[i].cell[0],cellsAb[i].cell[1],cellsAb[i].cell[2]),IRT_GQ,cellsAb[i].n);
		}
		
		//contrainte 8 : maximization du nombre de cellules
		if (1)
		{
			for (int i=0;i<=cell_n ;i++)
			{
					if (cellsAb[i].size == 1)
					count(*this,d,cellsAb[i].cell[0],IRT_EQ,costs[i]);
				if (cellsAb[i].size == 2)
					count(*this,cells2,cell2encode(cellsAb[i].cell[0],cellsAb[i].cell[1]),IRT_EQ,costs[i]);
				if (cellsAb[i].size == 3)
					count(*this,cells3,cell3encode(cellsAb[i].cell[0],cellsAb[i].cell[1],cellsAb[i].cell[2]),IRT_EQ,costs[i]);
			}		
			linear(*this, costs, IRT_EQ, k);
		}
		// contrainte 9 : Constrain them to be between -11 and 11
		// dom(*this, d, -11, 11);
		
		//distinct(*this, x, opt.ipl());
		//distinct(*this, d, opt.ipl());
		
		
		branch(*this, x, INT_VAR_SIZE_MIN(), INT_VAL_SPLIT_MIN());
	}
  /// Constructor for cloning \a e
  Jarrell(Jarrell& e)
    : IntMaximizeScript(e) {
    x.update(*this, e.x);
		k.update(*this, e.k);
  }
  /// Copy during cloning
  virtual Space*
  copy(void) {
    return new Jarrell(*this);
  }

/// Print solution
  virtual void
	print(std::ostream& os) const {
		std::ostringstream outputstring;
		const int n = x.size();
		os << "\tx[" << n << "] = {";
		outputstring << "(";
		output_file << "( (";
		for (int i = 0; i < n; i++)
		{
			os << x[i] << " ";
			outputstring << x[i] << " ";
			output_file << x[i] << " ";
		}
		os  << "}" << std::endl;
		outputstring << ")";
		output_file << ")";
		
		os  << "nombre de cellules effectifs (maximized)  =  " << k << std::endl;
		output_file  << k << " )" << std::endl;

			strcpy(result,outputstring.str().c_str());
		
	}
	
	/// Return solution cost for Maximization
	virtual IntVar cost(void) const {
		return k;
	}
		
};


/** \brief Main-function
 *  \relates Jarrell
 */



#if SCRIPT
int main(int argc, char* argv[]){
#else
int main_jarrell (int argc, char* argv[]){
#endif
	// SizeOptions opt("Jarrell");
	JarrellOptions opt("Jarrell","((1 3)(-3 -1))","(60 61 62 63 64 65)",0,0);
	
	opt.size(15);
	opt.iterations(5);
	opt.solutions(10);
	opt.ipl(IPL_BND);
	opt.model(Jarrell::MODEL_ITV);
	opt.model(Jarrell::MODEL_SIMPLE, "simple", "modele de base");
	opt.model(Jarrell::MODEL_ITV, "redondant", "tous les intervalles du résultat appartiennent aux cellules souhaitées");
	opt.search(Jarrell::SEARCH_BAB);
	opt.search(Jarrell::SEARCH_BAB,"bab");
	opt.search(Jarrell::SEARCH_DFS,"dfs");
	opt.search(Jarrell::SEARCH_RESTART,"restart");
	opt.search(Jarrell::SEARCH_BEST,"best");
	opt.xfirst = 0;
	opt.xlast = 0;
	opt.parse(argc, argv);
	if (opt.size() < 2) {
		std::cerr << "size must be at least 2!" << std::endl;
		return -1;
	}
	
	std::cout << "********************"<< std::endl;
	switch (opt.search()){
	case Jarrell::SEARCH_DFS:
		IntMaximizeScript::run<Jarrell,DFS,JarrellOptions>(opt);break;
	case Jarrell::SEARCH_BAB:
		IntMaximizeScript::run<Jarrell,BAB,JarrellOptions>(opt);break;
//	case Jarrell::SEARCH_RESTART:
//		IntMaximizeScript::run<Jarrell,Restart,JarrellOptions>(opt);break;
		case Jarrell::SEARCH_BEST:
	//		IntMaximizeScript::run<Jarrell,bab,JarrellOptions>(opt);
			break;


	}
	return 0;
}

//"main" functions to be called in the framework version :

extern "C" {
char *jarrellD(int n,char *str1,char *str2,int a,int b,char *resultat)
	{ 
		JarrellOptions opt("Jarrell","((1 3)(-3 -1))","(60 61 62 63 64 65)",0,0);
		
		opt.iterations(5);
		opt.solutions(1);
		opt.ipl(IPL_BND);
		opt.model(Jarrell::MODEL_ITV);
		opt.model(Jarrell::MODEL_SIMPLE, "simple", "modele de base");
		opt.model(Jarrell::MODEL_ITV, "redondant", "tous les intervalles du résultat appartiennent aux cellules souhaitées");

		//	Parsing the arguments :
		opt.size(n);
		opt._cells.append(str1);
		opt._chord.append(str2);
		opt.xfirst = a;
		opt.xlast = b;

		opt.derive();
		
		if (opt.size() < 2) {
			std::cerr << "size must be at least 2!" << std::endl;
			return NULL;
		}		
		strcpy(result,"nil");
		
		
		// Example::run<Jarrell,DFS,JarrellOptions>(opt);
		// Example::run<Jarrell,BAB,SizeOptions>(opt);
		IntMaximizeScript::run<Jarrell,BAB,JarrellOptions>(opt);
	
		sprintf(resultat, "%s", result);
		return result;
	}
};

extern "C" {
	char *jarrellE(int n,char *str1,char *str2,int a,int b,int n_solutions, char *filename,char *resultat)
	{ 
	//		JarrellOptions opt("Jarrell",str1,str2,a,b);
		JarrellOptions opt("Jarrell","((1 3)(-3 -1))","(60 61 62 63 64 65)",0,0);
		
		opt.iterations(5);
		opt.solutions(n_solutions);
		opt.ipl(IPL_BND);
		opt.model(Jarrell::MODEL_ITV);
		opt.model(Jarrell::MODEL_SIMPLE, "simple", "modele de base");
		opt.model(Jarrell::MODEL_ITV, "redondant", "tous les intervalles du résultat appartiennent aux cellules souhaitées");

		opt.size(n);
		opt._cells.append(str1);
		opt._chord.append(str2);
		opt.xfirst = a;
		opt.xlast = b;
		
		opt.derive();
		
		if (opt.size() < 2) {
			std::cerr << "size must be at least 2!" << std::endl;
			return NULL;
		}		
		strcpy(result,"nil");
		output_file.open(filename);
		//output_file.open("/Users/lemouton/Desktop/tmp",std::ios::out);
		
	if (output_file.is_open())
	{
		std::cerr << "output in result.tmp" << std::endl;
		output_file << ";; jarrell results : nombre de cellules \n";
	}	
	else
		
		std::cerr << "can't open " << filename << std::endl;
		// Example::run<Jarrell,DFS,JarrellOptions>(opt);
		IntMaximizeScript::run<Jarrell,BAB,JarrellOptions>(opt);
		sprintf(resultat, "%s", result);
		output_file.close();

		return result;
	}
};


extern "C" {
	char *jarrellG(int n,char *str1,char *str2,int a,int b,int n_solutions, char *filename,int timelimit, int model, int searchengine,char *resultat)
	{ 
		JarrellOptions opt("Jarrell","(1(1))","()",0,0);
		
		opt.iterations(5);
		opt.solutions(n_solutions);
		opt.ipl(IPL_BND);
		opt.model(model);
		opt.model(Jarrell::MODEL_SIMPLE, "simple", "modele de base");
		opt.model(Jarrell::MODEL_ITV, "redondant", "tous les intervalles du résultat appartiennent aux cellules souhaitées");
		
		opt.search(searchengine);
		opt.search(Jarrell::SEARCH_BAB,"bab");
		opt.search(Jarrell::SEARCH_DFS,"dfs");
		opt.search(Jarrell::SEARCH_RESTART,"restart");
		opt.search(Jarrell::SEARCH_BEST,"best");
		
		opt.time(timelimit);
		
		opt.size(n);
		opt._cells.append(str1);
		opt._chord.append(str2);
		opt.xfirst = a;
		opt.xlast = b;
		
		opt.derive();
		
		
		if (opt.size() < 2) {
			std::cerr << "size must be at least 2!" << std::endl;
			return NULL;
		}		
		strcpy(result,"nil");
		output_file.open(filename);
		//output_file.open("/Users/lemouton/Desktop/tmp",std::ios::out);
		
		if (output_file.is_open())
		{
			std::cerr << "output in result.tmp" << std::endl;
			output_file << ";; jarrell results : nombre de cellules \n";
		}	
		else
			
			std::cerr << "can't open " << filename << std::endl;
		
		switch (opt.search()){
			case Jarrell::SEARCH_DFS:
				IntMaximizeScript::run<Jarrell,DFS,JarrellOptions>(opt);break;
			case Jarrell::SEARCH_BAB:
				IntMaximizeScript::run<Jarrell,BAB,JarrellOptions>(opt);break;
	//		case Jarrell::SEARCH_RESTART:
	//			IntMaximizeScript::run<Jarrell,Restart,JarrellOptions>(opt);break;
			case Jarrell::SEARCH_BEST:
				//		MaximizeScript::run<Jarrell,bab,JarrellOptions>(opt);
				break;
		}		
				sprintf(resultat, "%s", result);
				output_file.close();
				
				return result;
		}
		
	};


// simple tests just to test communication between lisp and the library :
extern "C" 
{void random_string(int length, char *string)
{
	int ii;
	for (ii = 0; ii < length ; ii++)
		//string[ii] = 97 + rand() % 26;
		string[ii] = 98 ;
	string[length] = 0;
}
};

extern "C" 
{void modify(char *string) {
	char temp[256];
	sprintf(temp, "'%s' modified in a C function", string);
	strcpy(string, temp);
}
};


extern "C" 
{void concatene(char *str1,char *str2,int a,int b,char *string) {
	char temp[256];
	strcpy(temp,str1);
	strcat(temp,str2);
	//sprintf(temp, "'%s' modified in a C function", string);
	strcpy(string, temp);
}
};

int main_to_test(int argc, char* argv[]){
    		char b[200000];
//	jarrellD(16, "((1(-3 -8))(1(3 8))(1(6 -6)))","(60 65 66 68 71 76 82)",60,65,"resresresresres");
//	jarrellD(15, "((1(-3 -8))(1(3 8))(1(6 -6))(1(-6 6))(1(11 -11))(1(-11 11)(1(-3 11)))","(60 65 66 68 71 76 82)",0,0,"resresresresres");
//	jarrellE(16, "((1(-3 -8))(1(3 8))(1(6 -6)))","(60 65 66 68 71 76 82)",0,0,4,"/Users/lemouton/Desktop/resultjarrell.tmp","resresresresres");
jarrellG(16, "((1(-3 -8))(1(3 8))(1(6 -6)))","(60 65 66 68 71 76 82)",0,0,4,"/Users/lemouton/Desktop/resultjarrell.tmp",1000,1,1,b);
//	jarrellG(3, "((1(1))","()",0,0,4,"/Users/lemouton/Desktop/resultjarrell.tmp",1000,1,1,"resresresresres");

	std::cerr << "resultat " << b << std::endl;
//		std::cerr << "output string is " << xxx << std::endl;
	return 1;
}

// EXAMPLES OF PROBLEMS

// example A :

//jarrell 16 "((1(-3 -8))(1(3 8))(1(6 -6)))" "(60 65 66 68 71 76 82)" 0 0 -solutions 1
//********************
//Jarrell
//x[15] = {60 68 65 76 65 68 76 65 71 65 71 60 71 68 60 }
//cost k7
//x[15] = {60 68 65 76 65 68 76 65 76 65 71 65 71 68 60 }
//cost k8
//x[15] = {71 65 71 65 71 68 60 68 65 76 65 76 65 68 76 }
//cost k9
//
//Initial
//propagators:  42
//branchings:   1
//
//Summary
//runtime:      1:27.967 (87967.729000 ms)
//solutions:    3
//propagations: 102909838
//nodes:        5440751
//failures:     2720373
//peak depth:   32
//peak memory:  228 KB
//
//////////////////////////////////////////////////////
// example B1 :

//jarrell 16 "((1(-11 13 1))(1(13 -11))(1(16 -14 1)))" "(37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81)" 38 0 -solutions 2
//x[16] = {38 39 40 41 42 43 44 45 46 59 48 61 62 78 64 65 }
//nombre de cellules effectifs (maximized)  =  3
//x[16] = {38 39 40 41 42 43 44 57 46 59 48 61 62 78 64 65 }
//nombre de cellules effectifs (maximized)  =  4
//x[16] = {38 39 40 41 42 55 44 57 46 59 48 61 62 78 64 65 }
//nombre de cellules effectifs (maximized)  =  5
//x[16] = {38 39 40 53 42 55 44 57 46 59 48 61 62 78 64 65 }
//nombre de cellules effectifs (maximized)  =  6
//x[16] = {38 51 40 53 42 55 44 57 46 59 48 61 62 78 64 65 }
//nombre de cellules effectifs (maximized)  =  7
//
//Initial
//propagators:  49
//branchings:   1
//
//Summary
//runtime:      46.106 (46106.827000 ms)
//solutions:    5
//propagations: 146321597
//nodes:        7091451
//failures:     3545721
//peak depth:   30
//peak memory:  108 KB


//// example B1 : THIS IS A DIFFICULT ONE:
//jarrell 20 "((2(-11 13 1))(4(13 -11))(2(16 -14 1)))" "(37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81)" 0 0 -solutions 1
//x[20] = {37 38 51 40 53 42 55 44 57 58 74 60 61 77 63 64 77 66 79 80 }
//nombre de cellules effectifs (maximized)  =  8
//
//Initial
//propagators:  61
//branchings:   1
//
//Summary
//runtime:      10:50.684 (650684.945000 ms)
//solutions:    1
//propagations: 688815970
//nodes:        37372477
//failures:     18686226
//peak depth:   43
//peak memory:  257 KB
//

//////////////////////////////////////////////////////
// example C :
// jarrell 20 "((1(3 8))(1(6))(1(11))(1(-6))(1(-11)))" "(36 39 41 42 47 48 51 53 54 59 60 63 65 66 71 72 75 77 78 83 84)" 0 0 -time 15000
