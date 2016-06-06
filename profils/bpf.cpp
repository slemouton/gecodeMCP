/*
 *  bpf.cpp
 *  profils
 *
 *  Created by serge lemouton on 29/01/10.
 *  Copyright 2010 ircam. All rights reserved.
 *
 */

#include <iostream>
#include <string>
#include "bpf.h"

#define SHOW(I,X)   cout << "[" << (I) << "]\t " # X " = \"" << (X) << "\"" << endl

using namespace std;


void
split( vector<string> & theStringVector,  /* Altered/returned value */
	  const  string  & theString,
	  const  string  & theDelimiter )
{
	//	UASSERT( theDelimiter.size(), >, 0 ); // My own ASSERT macro.
	
	size_t  start = 0, end = 0;
	
	while ( end != string::npos )
	{
		end = theString.find( theDelimiter, start );
		
		// If at end, use length=maxLength.  Else use length=end-start.
		theStringVector.push_back( theString.substr( start,
													(end == string::npos) ? string::npos : end - start ) );
		
		// If at end, use start=maxSize.  Else use start=end+delimiter.
		start = (   ( end > (string::npos - theDelimiter.size()) )
				 ?  string::npos  :  end + theDelimiter.size()    );
	}
}


Bpf::Bpf(float table[],size_t length)
{
	for (int i = 0;i < length;i = i+2)
	{
		XX.push_back(table[i]);
		YY.push_back(table[i+1]);
	}
	scale();

}

Bpf::Bpf(string strin)
{		
	vector<string> v;
	
	//supprime les parentheses et les espaces en trop ...
	
	for(int i = strin.find("(", 0); i != -1; i = strin.find("(", i))
	{
		strin.replace(i,1," ");
		i++;  // Move past the last discovered instance to avoid finding same
	}
	for(int i = strin.find(")", 0); i != -1; i = strin.find(")", i))
	{
		strin.replace(i,1," ");
		i++;  // Move past the last discovered instance to avoid finding same
	}
	
	for(int i = strin.find(" ", 0); i != -1; i = strin.find(" ", i))
	{
		if (strin[i-1] == ' ')  //supprime les espaces en trop
		{
			strin.erase(i,1);
			i--;
		} 
		i++;  
	}
	
	//then tokenize
	
	split( v, strin, " " );
	
	
	for( unsigned int i = 1;  i < v.size() - 1; i=i+2 )
	{
		XX.push_back(atof(v[i].c_str()));
		YY.push_back(atof(v[i+1].c_str()));
	}
	scale();
}


Bpf::Bpf()
{
}

void Bpf::print(void)
{	
	std::cout << "print BPF : \n";
	for (unsigned i = 0;i<XX.size();i++)
		std::cout << XX[i] << " ";
	std::cout << std::endl;
	for (unsigned i = 0;i<XX.size();i++)
		std::cout << YY[i] << " ";
	std::cout << std::endl;
	
	
}
void Bpf::scale(void)
{
	float Xmax = XX.back();
	for (unsigned i = 0;i<XX.size();i++)
		XX[i]=XX[i]/Xmax;
}

vector<int> Bpf::profil(int n)
{
	vector<float> sampledfun;
	vector<int> result;
	sampledfun = sampleFun(n + 1);
	for (int i = 0;i < n ;i++)
	{
		if (sampledfun[i+1] > sampledfun[i])
			result.push_back(1);
		else
			result.push_back(0);

	}
	
	std::cout << "profil :";
		for (unsigned i = 0;i<result.size();i++)
		std::cout << result[i] << " ";
	std::cout << std::endl;

	return (result);
}

int Bpf::getIndex(float Xf)
{
	for (unsigned i = 0; i<XX.size(); i++)
		if(XX[i] > Xf)
		{
			return (i - 1);
		}
}

float Bpf::getYAt(float Xf) // linear interpolation !!
{
	int i = getIndex(Xf);
	float X0 = XX[i];
	float X1 = XX[i+1];
	float Y0 = YY[i];
	float Y1 = YY[i+1];
	float result = Y0 + (Xf - X0)*(Y1 - Y0)/(X1 - X0);
	//	cout <<"getYAT "<< Xf << " "<< X1 << " "<< Y0 << " "<< Y1 << " "<<result << std::endl;
	return result;
}

vector<float> Bpf::sampleFun(int n) // linear interpolation !!
{
	vector<float> result;
	for (unsigned i = 0;i<n;i++)
	{
		result.push_back(getYAt(i / (float) n));
	}
	return result;
}

