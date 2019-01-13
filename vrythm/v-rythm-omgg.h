/*
 *  v-rythm-omgg.cpp
 *
 *  Created by serge lemouton on 14/10/09.
 *  Copyright 2009 ircam. All rights reserved.
 *
 */

/// search variants
enum {SEARCH_DFS,   //use depth first search algorithm
	SEARCH_BAB,     // use branch and bound to optimize
	SEARCH_RESTART,  /// use restart to optimize 
	SEARCH_BEST  /// use restart to optimize 
};
#define MAXSTRINGLENGTH 200000 
#define MAXDIVISION 204800 
#define MAXN 256 
// #define MAXBEATS 100000 

char result[MAXSTRINGLENGTH];
//std::ofstream output_file;
std::ostringstream outputstring;

