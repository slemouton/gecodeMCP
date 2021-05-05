/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Serge lemouton@ircam.fr *
 */

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

#include <cstdlib>

#include <fstream>
#include <list>

#include "v-rythm-omgg.h"
#include "bpf.h"


using namespace Gecode;

class Chord {
public:
	int size;
	int octaviation; // 0/1 
	int chord[48];
};

enum {
    MODEL_ONE,
    MODEL_TIES,
    MODEL_BPF,
    MODEL_DIVN
    
};

std::list<int> divisions_l; // liste des divisions


static	Chord startA1={3,0,{1,2,3}};
static	Chord ndivA1={3,0,{1,2,3}};
static	Chord ndivA2={3,0,{1,2,3}};
static  int profil_i[MAXN];
static vector<int> profil_neume; // +1 ascendant, 0 descendant

class vrythmOptions : public Options {
    private:
    Driver::UnsignedIntOption _beats; // nombre de temps total
    Driver::UnsignedIntOption _beatmin; // nombre de temps minimal (optionnel)
    Driver::UnsignedIntOption _first; // premiere duree
    Driver::UnsignedIntOption _last; // derniere duree
    Driver::UnsignedIntOption _nvalues; // nombre de durées differentes utilisées
    Driver::StringValueOption _bpf;  // bpf for acceleration profile
    Driver::StringValueOption _divisionsNmin;  // nombre minimal autorisé pour chaque division
    Driver::StringValueOption _divisionsNmax;  // nombre minimal autorisé pour chaque division
    Bpf thebpf;
public:
    Driver::StringValueOption _divisions; // liste des divisions
    Driver::UnsignedIntOption _n; // nombre de temps total
    vector<float> profil;
    vector<float> profilb;


    
    //initialise les options
    vrythmOptions(const char *s,char *str1,const char *str2,const char *str3, const char* bpf0,int n0=12,int beats0=12,int beatmin0=0,int first0=0,int last0=0,int nvalues=0)
    :Options(s),
    _beats("-beats","nombre total de temps",beats0),
    _beatmin("-beatmin","nombre minimal de temps (optionnel)",beatmin0),
    _n("-n","nombre total d'impulsions",n0),
    _first("-first","premiere duree",first0),
    _last("-last","derniere duree",last0),
    _divisions("-divisions","divisions autorises",str1),
    _divisionsNmin("-divisionsNmin","nombre minimal autorisé pour chaque division",str2),
    _divisionsNmax("-divisionsNmax","nombre maximal autorisé pour chaque division",str3),
    _nvalues("-nvalues","nombre de durees differentes minimal",nvalues),
    _bpf("-bpf","breakpoint function for acceleration profile",bpf0)
   {
        //ajoute les options
       add(_beats);add(_beatmin);add(_n);add(_divisions);add(_first);add(_last);add(_nvalues);add(_bpf);add(_divisionsNmin);add(_divisionsNmax);
    }
	/// Parse options from arguments \a argv (number is \a argc)
	void parse(int& argc, char* argv[]) {
		Options::parse(argc,argv);
	}
   	int n(void) const { return _n.value();}
    int size(void) const {return _n.value();}
    int beats(void) const {return _beats.value();}
    int beatmin(void) const {return _beatmin.value();}
    int first(void) const {return _first.value();}
    int last(void) const {return _last.value();}
    int nvalues(void) const {return _nvalues.value();}

    bool divisions(void) const { return  _divisions.value(); }
    bool divisionsNmin(void) const { return  _divisionsNmin.value(); }
    bool divisionsNmax(void) const { return  _divisionsNmax.value(); }
  
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
			//		std::cout << "debug in assign chord note : " << chord_note << std::endl;
			}
			
			//		if ((XMIN < chord_note) && (chord_note < XMAX))
			result->chord[j++] = chord_note;
		}
		result->size = chord_size;		
	}
	
    void print_possible_divisions(void)
    {
        std::list<int>::iterator it;
        std::cout << "possible divisions";
        for ( it=divisions_l.begin() ; it != divisions_l.end(); it++ )
            std::cout << " " << *it;
        std::cout << std::endl;
    }
    
	void init_divisions(void)
	{
        divisions_l.clear();
        assign_arg_string(_divisions.value(),&startA1);
        //      std::cout << "nombre de notes " << startA1.size << std::endl;
        
        //Calcul des liaisons (deux à deux pour le moment)
        for (int i = 0; i<startA1.size - 1; i++) 
            divisions_l.push_front(startA1.chord[i]);
        if (model() == MODEL_TIES)
            
            for (int i = 0; i<startA1.size - 1; i++) 
                for (int j = 0; j<startA1.size - 1; j++) 
                    divisions_l.push_front(startA1.chord[i]+startA1.chord[j]);
        
        divisions_l.sort();
        if (divisions_l.back() > MAXDIVISION)
        {std::cout << "MAXDIV depassé !!!";
            exit (0);
        }
        divisions_l.unique();
        
//        print_possible_divisions();

	}
    
  
    
    void init_divN(void)
        {
            assign_arg_string(_divisionsNmin.value(),&ndivA1);
            assign_arg_string(_divisionsNmax.value(),&ndivA2);
        }
    
    void init_bpf(void)
    {
        
 		int i = 0,val;
        int min = divisions_l.front();
        int max = divisions_l.back();
        std::cout << "compute problem input data : " << _bpf.value() << " - " << std::endl;
		string str1 = _bpf.value();
		Bpf bpf(str1);
		thebpf=bpf;
        profil = bpf.sampleFun(size());
//     profilb = bpf.sampleFun(beats());

        bpf.print();
 
        vector<float>::iterator it;
        cout << "profil contains:";
        for ( it=profil.begin() ; it < profil.end(); it++ )
        {    val = (int) (*it * (max-min) + min);
            profil_i[i++] = val;
            cout  << " (" << *it<< ") ";
            cout << " "<< val;
        }
        cout << endl;
#if 0     
        cout << "profilb contains:";
        for ( it=profilb.begin() ; it < profilb.end(); it++ )
        {    val = (int) (*it * (max-min) + min);
            profil_i[i++] = val;
    //        cout  << " (" << *it<< ") ";
     //       cout << " "<< val;
        }
        cout << endl;
#endif

        profil_neume = bpf.profil(size());
        vector<int>::iterator it2;
      
        i=0;
         cout << "neume contains:";
        for ( it2=profil_neume.begin() ; it2 < profil_neume.end(); it2++ )
        {   
            //val = (int) (*it * (max-min) + min);
            //profil_i[i++] = val;
            cout  <<profil_neume[i++];
           
        }
        cout << endl;

        
    }
    void auto_n(void)
    {
        // calcul automatique du nombre de divisions
        int nmin,nmax;
        nmax = beats() / divisions_l.front();
        nmin = beats() / divisions_l.back();
       
        cout << "n entre "<< nmin<< " et "<< nmax <<std::endl;
        _n.value(nmax);
        
        divisions_l.push_front(0);
         print_possible_divisions();
        
       if (model() == MODEL_DIVN)
           {
               
               ndivA1.chord[ndivA1.size] = 0;
               ndivA1.size++;
               
               ndivA2.chord[ndivA2.size] = nmax-nmin;
               ndivA2.size++;
           }
        
    }


 
};

class VRythm : public IntMinimizeScript {
private:
  /// The numbers
  IntVarArray x;
    /// the cost
    IntVar k;
public:
	

	
  /// Actual model
  VRythm(const vrythmOptions& opt) :
    IntMinimizeScript(opt),
    x(*this, opt.size(), 0, MAXDIVISION),
    k(*this,0,1000000)
    { 
		const int n = x.size();
		
		// IntVarArgs d(n-1);
		IntVarArgs dd(*this,n,0,MAXDIVISION);
		//IntVarArgs xx(*this,startA1.size - 1,0,MAXDIVISION);
        IntVarArgs xx(*this,divisions_l.size(),0,MAXDIVISION);
		IntVarArgs xxx(*this,divisions_l.size(),0,MAXDIVISION);
        IntVarArgs xsums(*this,n,0,MAXDIVISION); /* sommes incrementales */
        IntVarArray profil_b(*this,opt.beats(),0,1000);
        Rnd r(1U);
        
	if (opt.model() == MODEL_TIES) // autorise liaisons des durées (2 par 2)
		{
            //THE RULES :
            
            //1.domaine (durée données en entrée)
          /*  for (int i = 0; i<startA1.size - 1; i++) {
                rel(*this, xx[i],IRT_EQ,startA1.chord[i]);
            }
            for (int i = 0; i<n; i++)
                member(*this,xx,x[i]);
           */
            // 1b.domaine (liaison possibles : additions des durées 2 a 2)

            std::list<int>::iterator it;
            int i =0;
            for ( it=divisions_l.begin() ; it != divisions_l.end(); it++ )
                 rel(*this, xxx[i++],IRT_EQ, *it);
   
            for (int i = 0; i<n; i++)
                member(*this,xxx,x[i]);
            
                
		}
else 			
		{
            
            //THE RULES :

           //1.domaine
            
#if 0
            for (int i = 0; i<startA1.size - 1; i++) {
                rel(*this, xx[i],IRT_EQ,startA1.chord[i]);
            }
            
#endif
           
            std::list<int>::iterator it;
            int i =0;
            for ( it=divisions_l.begin() ; it != divisions_l.end(); it++ )
                rel(*this, xx[i++],IRT_EQ, *it);
            
           for (int i = 0; i<n; i++)
               member(*this,xx,x[i]);

         
            
		}
        //2.durée totale
        if (opt.beatmin() != 0)
        {
            rel(*this,sum(x) <= opt.beats());
            rel(*this,sum(x) >= opt.beatmin());
        }
        else
            rel(*this,sum(x) == opt.beats());
        
        //3.valeur initiale
        if (opt.first() != 0 ) 
        {rel(*this, x[0],IRT_EQ,opt.first());}
        
        //4.valeur finale
        if (opt.last() != 0 ) 
        {rel(*this, x[n-1],IRT_EQ,opt.last());}
        
        //5.ordre
        if (opt.model() != MODEL_BPF)  
            for (int i=0; i<n-1; i++)
                rel(*this, (x[i+1] >= x[i]), opt.ipl());
        
        //6.nombre de dureees differentes
        if (opt.nvalues() !=0)
        {
            nvalues(*this, x, IRT_GQ, opt.nvalues());
        }
        
        //7.occurence de chaque duree differente     
        if (opt.model() == MODEL_DIVN)  
            {
                for (int i = 0; i<ndivA1.size - 1; i++) {
                    count(*this, x,startA1.chord[i],IRT_GQ,ndivA1.chord[i]);
                }  
                
                for (int i = 0; i<ndivA2.size - 1; i++) {
                    count(*this, x,startA1.chord[i],IRT_LQ,ndivA2.chord[i]);
                }  
            }
        
        //8.compare le resulat avec un modele d'acceleration representé par une bpf     
        if (opt.model() == MODEL_BPF)  
        {
#if 1
            for (int i=0; i<n-1; i++)
            {
                if(profil_neume[i] == 0)
                    rel(*this, (x[i+1] <= x[i]), opt.ipl());   
                else
                    rel(*this, (x[i+1] >= x[i]), opt.ipl());
            }
#endif

#if 1
            for (int i = 0; i<n  ; i++) {
               dd[i] = expr(*this,abs(x[i] - profil_i[i]));      
             //square :
               // dd[i] = expr(*this,(x[i] - profil_i[i])* (x[i] - profil_i[i]));
            }
            
            linear(*this, dd, IRT_EQ, k);
            
#endif     
#if 0
           // xsums[0] = expr(*this,  x[0],opt.ipl());
            for (int i = 0; i<n; i++)
                //xsums[i] = expr(*this,xsums[i-1] + x[i],opt.ipl());
                expr(*this,xsums[i] == 1);
            for (int i = 0; i<opt.beats(); i++)
                rel(*this,profil_b[i],IRT_EQ,opt.profilb[i]);
            for (int i = 0; i<n  ; i++) {
                IntVar yyy;
  //              element(*this,profil_b,xsums[i],yyy);
                yyy = expr(*this,element(profil_b,xsums[i]));
                dd[i] = expr(*this,abs(x[i] - yyy));      
            }
            
            linear(*this, dd, IRT_EQ, k);
            
#endif

        }
        
            
		if(opt.branching() == 0)
			branch(*this, x, INT_VAR_SIZE_MIN(), INT_VAL_SPLIT_MIN());
		else 
			branch(*this, x, INT_VAR_RND(r), INT_VAL_RND(r));

		
	}
	
  /// Constructor for cloning \a e
  VRythm(VRythm& e)
    : IntMinimizeScript(e) {
    x.update(*this, e.x);
        k.update(*this, e.k);
  }
  /// Copy during cloning
  virtual Space*
  copy(void) {
    return new VRythm(*this);
  }
  /// Print solution
  virtual void
	print(std::ostream& os) const {

		const int n = x.size();
		os << "\tx[" << n << "] = {";
		outputstring << "(";
		for (int i = 0; i < n-1; i++)
		{
			os << x[i] << ",";
			outputstring << x[i] << " ";
		}
        outputstring << x[n-1]<<  ")";
		os << x[n-1] << "}" << std::endl;

#if 0        
        os << "\txsums[" << n << "] = {";

        for (int i = 0; i < n-1; i++)
		{
			os << xsums[i] << ",";
			outputstring << x[i] << " ";
		}
        	os << xsums[n-1] << "}" << std::endl;
#endif
        
        os  << "distance (Minimized)  =  ";
         os  << k << std::endl;
		
	}
    
    /// Return solution cost for Minimization
	virtual IntVar cost(void) const {
		return k;
	}

};


/** \brief Main-function
 *  \relates VRythm
 */
#if SCRIPT
int main(int argc, char* argv[]){
#else
int main_vrythm(int argc, char* argv[]){
#endif

	vrythmOptions opt("VRythm","( 5 4 )","()","()","()",12,12);
	opt.iterations(5);
	opt.ipl(IPL_BND);
	opt.model(0);
	opt.model(MODEL_ONE, "one", "verunelli probleme numero un");
	opt.model(MODEL_TIES,"ties", " autorise les liaisons (2 a 2)");
	opt.model(MODEL_BPF,"bpf", "utilise le profil melodique");
	opt.model(MODEL_DIVN,"div_n", "controle le nombre minimal d'occurence de chaque duree");
		
	opt.symmetry(0);
	opt.symmetry(0, "no", "calculate all the solutions");
	opt.symmetry(1, "yes", "calculate only the caracteristic solutions");

	opt.branching(0); //normal
	opt.branching(0, "normal", "normal  branching");
	opt.branching(1, "random", "random  branching");

	
	opt.parse(argc, argv);
	if (opt.size() == 1) {
		std::cerr << "size must be at least 2!" << std::endl;
		return -1;
	}
	
    opt.init_divisions();
    
    if (opt.model() == MODEL_DIVN)
        opt.init_divN();
    
    if (opt.model() == MODEL_BPF)
        opt.init_bpf();
    
    if (opt.n() == 0)
        opt.auto_n();	
    
    if (opt.model() == MODEL_BPF)
        IntMinimizeScript::run<VRythm,BAB,vrythmOptions>(opt);
    else
        Script::run<VRythm,DFS,vrythmOptions>(opt);
    
	return 0;
}

// STATISTICS: example-any
// cf allinterval-omgg.cpp

extern "C" {
	char *vrythmG(int n, int beats,int beatmin,char *divisions,char *divisionsN0,char *divisionsN1,char *bpf,int first,int last,int n_solutions, int n_values, int branching,int model,int timelimit, char *resultat)
	{ 
        vrythmOptions opt("VRythm",divisions,divisionsN0,divisionsN1,bpf,n,beats,beatmin,first,last,n_values);

		std::string chaine;

		opt.iterations(5);
		opt.solutions(n_solutions);
		opt.ipl(IPL_BND);
        
  		
		// il peut y avoir plusieurs modelisation du même problème dans un script
        opt.model(model);
        opt.branching(branching);
		opt.time(timelimit);
		
		if (opt.size() == 1) {
			std::cerr << "size must be at least 2!" << std::endl;
			return NULL;
		}		
        
        
        std::cerr << "Running " << n << " "<< beats << " "<<divisions << " "<< first << " "<< last << " " << n_solutions<< " "<< opt.model() << " "<< divisionsN0<< " " <<divisionsN1 << std::endl;
        
        opt.init_divisions();
        
        if (opt.model() == MODEL_DIVN)
            opt.init_divN();
        
        if (opt.model() == MODEL_BPF)
            opt.init_bpf();
 
        if (opt.n() == 0)
            opt.auto_n();	
        
		strcpy(result,"nil");
		outputstring.str("x");
		outputstring << "(";
		       
        if (opt.model() == MODEL_BPF)
            IntMinimizeScript::run<VRythm,BAB,vrythmOptions>(opt);
        else
            Script::run<VRythm,DFS,vrythmOptions>(opt);
		
		
		outputstring << ")";
		//		std::cerr << "result " << outputstring.str().c_str() << std::endl;
		std::cerr << "length " << outputstring.str().size() << "/" <<MAXSTRINGLENGTH << std::endl;

#if 0
        strncpy(result,outputstring.str().c_str(),MAXSTRINGLENGTH );
#endif		
		//		std::cerr << "result " << result << std::endl;
		
		// pour éviter de déborder la chaine de sortie:
	
		chaine = outputstring.str();
		
		if (chaine.size() > MAXSTRINGLENGTH) {
			std::cerr << "result string is too long - ";
			chaine.resize(MAXSTRINGLENGTH);
		}
		
		strncpy(result,chaine.c_str(),MAXSTRINGLENGTH - 1);		
		
		// pour éviter de déborder la chaine de sortie(fin
		
		sprintf(resultat, "%s", result);
		
		std::cerr << "resultat " << resultat << std::endl;
		
		return result;
	}
	
};

/*to test*/
int
main_to_test(int argc, char* argv[]){
	char *a = "( 1 2 4 0 )";
		char b[200000];
//	vrythmG(8,36,0,a,0,0,0,0, 0,b);
 //   vrythmG(120,120,0,a,0,0,1,0, 0,b);
  //     vrythmG(120,120,120,a,0,0,5,0,0,1, 0,b);
	std::cerr << "resultat " << b << std::endl;

	return 0;
}



