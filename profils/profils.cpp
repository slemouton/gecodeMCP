/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */


#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/set.hh>

#include "bpf.h";

using namespace Gecode;

/**
 * \brief %Options for %Profils problems
 *
 */

//char result[] = "le resultat est                                                                         ";
char result[8192];

class ProfilsOptions : public Options {
private:
	/// Number of notes
	Driver::UnsignedIntOption _nn;
	/// Minimum distance
	Driver::UnsignedIntOption _distance;
	/// Number of symbols
	Driver::UnsignedIntOption _size;
	// breakpoints functions
	Driver::StringValueOption _bpfs;
	Bpf thebpfs[2];
	unsigned int bpfs_n;
	
public:
	vector<int> profils[2];
	unsigned int densMin;
	unsigned int densMax;

	
	/// Initialize options for example with name \a s
	ProfilsOptions(const char* s, unsigned int nn0,
				   unsigned int distance0, unsigned int densMin,unsigned int densMax,unsigned int size0,const char* bpfs0)
	: Options(s)
	, _nn("-nn","number of notes in the harmonic domain",nn0)
	, _distance("-distance","minimum distance",distance0)
	, _size("-size","number of chordsProfils",size0)
	, _bpfs("-bpfs","breakpoint functions for upper and lower voice",bpfs0){
		add(_nn);
		add(_distance);
		add(_size);
		add(_bpfs);
	}
	
	/// Return maximum number of notes
	unsigned int nn(void) const { return _nn.value(); }
	/// Return minimum distance
	unsigned int distance(void) const { return _distance.value(); }
	/// Return number of chords
	unsigned int size(void) const { return _size.value(); }
	// string bpfs(void) const {return _bpfs.value();}
	/// Derive the bpf
	void derive(void) {
		
		std::cout << "compute problem input data : " << _bpfs.value() << " - " << std::endl;
		string str1 = _bpfs.value();
		string str2 (") (");
		string sub1,sub2;
		size_t found;
		
		// different member versions of find in the same order as above:
		found=str1.find(str2);
		if (found!=string::npos)
			cout << "first ))(( found at: " << int(found) << endl;
		sub1 = str1.substr(1,found);
		sub2 =  str1.substr(found +1,str1.size() - found - 2);
		cout << " " << str1.substr(1,found) << endl;
		cout <<" " << str1.substr(found +1,str1.size() - found - 2) << endl;
		
		cout <<"SIZE " << _size.value() << endl;
		
		Bpf bpf0(sub1);
		thebpfs[0]=bpf0;
		profils[0]=bpf0.profil(size() - 1);
		
		Bpf bpf1(sub2);
		thebpfs[1]=bpf1;
		profils[1]=bpf1.profil(size() - 1);
		
		
		bpfs_n = 1;	
	}
	
};

/**
 * \brief %Example: Generating %Profils codes
 *
 * enchainement d'accords avec contraintes sur le profil
 * par exemple : la note inferieure monte tandis que la note superieure descend
 * ensuite le parcours est controlé par une bpf
 * \ingroup ExProblem
 *
 */
class Profils : public Script {
private:
	/// The profils code
	SetVarArray xs;
public:
	//int bpfs[1][12];
	/// Actual model
	Profils(const ProfilsOptions& opt) :
	  	Script(opt),
    xs(*this,opt.size(),IntSet::empty,1,opt.nn()) {
# if 0 // was the hamming constrain
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
				post(*this, xIntCyCard+yIntCxCard >= opt.distance());
			}
		}
#endif
		
		
		for (int i=0; i<(xs.size() - 1); i++)
		{
			IntVar min0(*this,0,120);
			IntVar min1(*this,0,120);
			IntVar max0(*this,0,120);
			IntVar max1(*this,0,120);
			
			
			min(*this,xs[i],min0);
			min(*this,xs[i+1],min1);
			if (opt.profils[0][i] == 1) 
				rel(*this, min1,IRT_GR, min0);
			else
				rel(*this, min1,IRT_LE, min0);
			
			max(*this,xs[i],max0);
			max(*this,xs[i+1],max1);
			if (opt.profils[1][i] == 1) 
				rel(*this, max1,IRT_GR, max0);
			else
				rel(*this, max1,IRT_LE, max0);
			
		}
	
		//contrainte de cardinalité
		IntVar card(*this,opt.densMin,opt.densMax);
		for (int i=0; i<(xs.size()); i++)
			cardinality(*this, xs[i], card);

		
		branch(*this, xs, SET_VAR_NONE(), SET_VAL_MIN_INC());
	}
	
	/// Print solution
	virtual void
	print(std::ostream& os) const {
		for (int i=0; i<xs.size(); i++) {
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
		
		std::cout << "OUTPUT" << outputstring.str();

		
		strcpy(result,outputstring.str().c_str());		
		
	}
	
	
	/// Constructor for copying \a s
	Profils(Profils& s) : Script(s) {
		xs.update(*this, s.xs);
	}
	/// Copy during cloning
	virtual Space*
	copy(void) {
		return new Profils(*this);
	}
	
};

/** \brief Main-function
 *  \relates Profils
 */
int
main(int argc, char* argv[]) {
	ProfilsOptions opt("Profils",20,3,3,5,32,"((O 1 2 3))");
	opt.parse(argc,argv);
	opt.derive();
	Script::run<Profils,DFS,ProfilsOptions>(opt);
	return 0;
}

/*bpf class tests*/

int main_testBPF(int argc, char* argv[]) {
	float vals[]={0,0,1,1,2,0};
	string s = "(0 0.5 1.222 1 2 2.23456)";
	Bpf test(vals,6);
	Bpf test2(s);
	
	vector<float> sampledfun;
	vector<int> prof;
	
	test.print();
	
	sampledfun = test.sampleFun(10);
	std::cout << "sampled :" << std::endl;
	for (int i = 0;i < sampledfun.size();i++)
		std::cout << " "<< sampledfun[i] << " ";
	std::cout << " " << std::endl;
	
	prof = test.profil(10);
	std::cout << "profil :" << std::endl;
	for (int i = 0;i < prof.size();i++)
		std::cout << " "<< prof[i] << " ";
	std::cout << " " << std::endl;
	
	test2.print();
	return 0;
}

/* function to be called in the framework version */


extern "C" {
	char *profilsG(int n,char *str1,int nn0, int densMin,int densMax, int n_solutions,int timelimit,char *resultat)
	{ 
		
		ProfilsOptions opt("Profils",nn0,3,densMin,densMax,n,str1);
		
		opt.iterations(5);
		opt.solutions(n_solutions);
		
		opt.time(timelimit);
//		opt.size(n);	
//		opt._cells.append(str1);
//		opt.nn (20);
		opt.densMin = densMin;
		opt.densMax = densMax;

		opt.derive();
		
		strcpy(result,"nilnil");
		
		Script::run<Profils,DFS,ProfilsOptions>(opt);
		
		sprintf(resultat, "%s", result);
		
		return result;
	}
	
};


int main_test(int argc, char* argv[]){
	char r[10000];
	profilsG(7,  "((0 1 1 0 2 1) (0 0 1 1 2 0))",20,2,5,1,0,r);
	
	std::cerr << "result string is " << result << std::endl;
	//		std::cerr << "output string is " << xxx << std::endl;
	return 1;
}

