#include "hw2.h"
#include <map>
#include <vector>
#include <iostream>
#include <cstdlib>

using std::vector;
using std::set;
using std::map;
typedef map<nonterminal , map<tokens, int> > MType;

#define ERROR -1

void compute_nullable() {
    vector<bool> v(NONTERMINAL_ENUM_SIZE, false);
	// Loop through all layers of the matrix
	for (int i = S; i < NONTERMINAL_ENUM_SIZE; ++i) {
		// Loop through all possible grammars
		for (int j = 0; j < grammar.size(); j++) {
			// Loop through all terminals of each rule
			bool is_nullable = true;
			for (int k = 0; k < grammar[j].rhs.size(); k++) {
				if (grammar[j].rhs[k] > NONTERMINAL_ENUM_SIZE || v[grammar[j].rhs[k]] == false) {
					is_nullable = false;
				}
			}
			if (is_nullable) {
				v[grammar[j].lhs] = true;
			}
		}
	}
	print_nullable(v);
}

void compute_first() {

}

void compute_follow() {

}

void compute_select() {

}

void parser() {

}