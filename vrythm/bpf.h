/*
 *  bpf.h
 *  profils
 *
 *  Created by serge lemouton on 29/01/10.
 *  Copyright 2010 ircam. All rights reserved.
 *
 */
#include <vector>
#include <string>
using namespace std;


class Bpf{
	
	vector<float> XX;
	vector<float> YY;
	string str;
	
	public :
	Bpf();
	Bpf(float *,size_t);
	Bpf(string);//constructors
	void print();
	void scale();
    void scale(float,float);
	vector<int> profil(int);
	vector<float> sampleFun(int);
	int getIndex(float);
	float getYAt(float);
};


