/*
 *  all-interval-omgg.cpp
 *  all-interval
 *
 *  Created by serge lemouton on 14/10/09.
 *  Copyright 2009 ircam. All rights reserved.
 *
 */
#include <string>
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
#define MAXSTRINGLENGTH 200000
static char result[MAXSTRINGLENGTH];
static std::ofstream output_file;
static std::ostringstream outputstring;

