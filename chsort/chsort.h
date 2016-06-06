/*
 *  chsort.h *
 *  Created by serge lemouton on 14/10/09.
 *  Copyright 2010 ircam. All rights reserved.
 *
 */

/// search variants
enum {SEARCH_DFS,   //use depth first search algorithm
	SEARCH_BAB,     // use branch and bound to optimize
	SEARCH_RESTART,  /// use restart to optimize 
	SEARCH_BEST  /// use restart to optimize 
};
#define MAXSTRINGLENGTH 400000
static char result[MAXSTRINGLENGTH];
static std::ofstream output_file;
static std::ostringstream outputstring;

int tester(void);

