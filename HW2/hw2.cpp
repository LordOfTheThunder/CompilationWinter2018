#include "hw2.h"
#include <map>
#include <vector>
#include <iostream>
#include <cstdlib>


using std::vector;
using std::set;
using std::map;
typedef map<nonterminal , map<tokens, int> > MType;

#define IS_NONE_TERMINAL(term) (((term) >= S) && ((term) < NONTERMINAL_ENUM_SIZE))
#define ERROR (-1)

vector<bool> get_nullable() {
	vector<bool> v(NONTERMINAL_ENUM_SIZE, false);
	// Loop through all layers of the matrix
	for (int i = S; i < NONTERMINAL_ENUM_SIZE; ++i) {
		// Loop through all possible grammars
		for (int j = 0; j < grammar.size(); ++j) {
			// Loop through all terminals of each rule
			bool is_nullable = true;
			for (int k = 0; k < grammar[j].rhs.size(); k++) {
				if (grammar[j].rhs[k] > NONTERMINAL_ENUM_SIZE || v[grammar[j].rhs[k]] == false) {
					is_nullable = false;
				}
			}
			v[grammar[j].lhs] = is_nullable;
		}
	}
	return v;
}

void compute_nullable() {
	print_nullable(get_nullable());
}

vector<set<tokens>> init_first() {
	// Start with already enough size to fit all first(A)=0 for nonterminals
	vector<set<tokens>> v(NONTERMINAL_ENUM_SIZE + 1);
	// Only calculate first for terminals
	for (int i = KEY; i <= EF; ++i) {
		set<tokens> s;
		s.insert(static_cast<tokens>(i));
		v.push_back(s);
	}
	return v;
}

vector<set<tokens>> first_step() {
	vector<set<tokens>> v = init_first();
	vector<bool> nullables = get_nullable();
	bool has_changes = true;
	while (has_changes) {
		has_changes = false;
		for (int i = 0; i < grammar.size(); ++i) {
			nonterminal lhs = grammar[i].lhs;
			for (int j = 0; j < grammar[i].rhs.size(); ++j) {
				if (std::find(v[lhs].begin(), v[lhs].end(), grammar[i].rhs[j]) != v[lhs].end()) {
					// element already found
					break;
				}
				bool inside_changes = true;
				for (auto const& value : v[grammar[i].rhs[j]]) {
					if (std::find(v[lhs].begin(), v[lhs].end(), value) == v[lhs].end()) {
						inside_changes = false;
						break;
					}
				}
				if (inside_changes) {
					break;
				}
				if (grammar[i].rhs[j] <= NONTERMINAL_ENUM_SIZE && v[grammar[i].rhs[j]].empty()) {
					break;
				}
				v[lhs].insert(v[grammar[i].rhs[j]].begin(), v[grammar[i].rhs[j]].end());
				has_changes = true;
				break;
			}
		}
	}
	return v;
}

void compute_first() {
	print_first(first_step());
}

static vector<set<tokens>> get_the_fellows(){
//  Init follow set
	vector<set<tokens>> fellows(NONTERMINAL_ENUM_SIZE);
	fellows[S].insert(EF);

	while (true) {
		bool changed = false;
		for (std::vector<grammar_rule>::iterator it = grammar.begin(); it != grammar.end(); ++it){
			grammar_rule& rule = *it; // rule
			std::vector<int>& rhs = rule.rhs; // right hand

			// Check for changes
			for (std::vector<int>::iterator rhs_it = rhs.begin(); rhs_it != rhs.end(); ++rhs_it){ // iterate over rule
				// *rhs_it is the current terminal/variable
				if (IS_NONE_TERMINAL(*rhs_it)){
					std::vector<int>::iterator rhs_next = rhs_it;
					++rhs_next;
					set<tokens> rhs_first = rangeFirst(rhs_next, rhs.end());
					if (!rhs_first.empty()){
						changed = true;
						// merge the first set into the current follow set
						fellows[*rhs_it].insert(rhs_next, rhs.end());
						if (rangeNullable(rhs_next, rhs.end())){
							fellows[*rhs_it].insert(fellows[rule.lhs].begin(), fellows[rule.lhs].end())
						}
					}

				}
			}
		}

		if (!changed)
			break;
	}

}

void compute_follow() {
    print_follow(get_the_fellows());
}

void compute_select() {

}

void parser() {

}

//
// Created by bachr on 21-Apr-18.
//

/*
// Nir's shit
#include "hw2.h"
#include <map>
#include <iostream>
#include <cstdlib>

using std::vector;
using std::set;
using std::map;
typedef map<nonterminal, map<tokens, int> > MType;

const int error = -1;

static inline bool isVariable(int t) {
	return t >= S && t < NONTERMINAL_ENUM_SIZE;
}

static vector<bool> getNullables() {
	vector<bool> v(NONTERMINAL_ENUM_SIZE, false);
	for (int i = 0; i < NONTERMINAL_ENUM_SIZE; ++i) {
		for (int j = 0; j < grammar.size(); ++j) {
			bool nullable = true;
			for (int k = 0; k < grammar[j].rhs.size(); ++k) {
				if (!isVariable(grammar[j].rhs[k]) || !v[grammar[j].rhs[k]]) {
					nullable = false;
				}
			}
			if (nullable) {
				v[grammar[j].lhs] = true;
			}
		}
	}
	return v;
}

void compute_nullable() {
	print_nullable(getNullables());
}

static inline bool isNullable(int t) {
	static const vector<bool> v = getNullables();
	return isVariable(t) && v[t];
}

static inline vector<set<tokens> > init_first() {
	vector<set<tokens > > v(NONTERMINAL_ENUM_SIZE + 1);
	for (int i = KEY; i <= EF; ++i) {
		set<tokens> stmp;
		stmp.insert((tokens)i);
		v.push_back(stmp);
	}
	return v;
}

static vector<set<tokens> > firstPhase1() {
	vector<set<tokens> > v = init_first();
	bool changed = true;
	while (changed) {
		changed = false;
		for (int i = 0; i < grammar.size(); ++i) {
			int j = 0;
			grammar_rule& rule = grammar[i];
			do {
				if (j >= rule.rhs.size())
					break;
				unsigned long long int size_before = v[rule.lhs].size();
				v[rule.lhs].insert(v[rule.rhs[j]].begin(),
					v[rule.rhs[j]].end());
				if (size_before != v[rule.lhs].size()) {
					changed = true;
				}
			} while (isNullable(rule.rhs[j++]));
		}
	}
	return v;
}

void compute_first() {
	print_first(firstPhase1());
}

static inline vector<set<tokens> > init_follow() {
	vector<set<tokens> > v(NONTERMINAL_ENUM_SIZE);
	v[S].insert(EF);
	return v;
}

template <class iterator>
static bool rangeNullable(iterator begin, iterator end) {
	while (begin != end) {
		if (!isNullable(*(begin++)))
			return false;
	}
	return true;
}

template <class iterator>
static set<tokens > rangeFirst(iterator begin, iterator end) {
	static const vector<set<tokens> > first_vec = firstPhase1();
	if (begin == end)
		return set<tokens >();
	set<tokens > s = first_vec[*begin];
	if (isNullable(*begin)) {
		set<tokens > temp = rangeFirst(++begin, end);
		s.insert(temp.begin(), temp.end());
	}
	return s;
}

#include <iostream>
static vector<set<tokens> > getFollorw() {
	vector<set<tokens> > v = init_follow();
	bool changed = true;
	while (changed) {
		changed = false;
		for (int i = 0; i < grammar.size(); ++i) {
			grammar_rule& rule = grammar[i];
			for (vector<int>::iterator it = rule.rhs.begin();
				it != rule.rhs.end(); ++it) {
				if (isVariable(*it)) {
					vector<int>::iterator it2 = it;
					set<tokens> temp_set = rangeFirst(++it2, rule.rhs.end());
					unsigned long long int size_before = v[*it].size();
					v[*it].insert(temp_set.begin(), temp_set.end());
					if (rangeNullable(it2, rule.rhs.end())) {
						v[*it].insert(v[rule.lhs].begin(), v[rule.lhs].end());
					}
					if (size_before != v[*it].size()) {
						changed = true;
					}
				}
			}
		}
	}
	return v;
}

void compute_follow() {
	print_follow(getFollorw());
}

static vector<set<tokens> > getSelect() {
	vector<set<tokens> > v;
	vector<set<tokens> > follow = getFollorw();
	for (int i = 0; i < grammar.size(); ++i) {
		grammar_rule& rule = grammar[i];
		v.push_back(rangeFirst(rule.rhs.begin(), rule.rhs.end()));
		if (rangeNullable(rule.rhs.begin(), rule.rhs.end())) {
			v.back().insert(follow[rule.lhs].begin(), follow[rule.lhs].end());
		}
	}
	return v;
}

void compute_select() {
	print_select(getSelect());
}
static MType getM() {
	vector<set<tokens> > select = getSelect();
	MType m;
	for (int non_term = S; non_term < NONTERMINAL_ENUM_SIZE; ++non_term) {
		for (int tok = KEY; tok < EF; ++tok) {
			m[(nonterminal)non_term][(tokens)tok] = error;
		}
	}
	for (int i = 0; i < grammar.size(); ++i) {
		grammar_rule& rule = grammar[i];
		for (set<tokens>::iterator it = select[i].begin();
			it != select[i].end(); ++it) {
			if (m[rule.lhs][*it] != error) {
				std::cout << "A conflict detected in variable " << rule.lhs
					<< " between rules " << m[rule.lhs][*it] << " and " << i
					<< " with the token " << *it << std::endl;
			}
			m[rule.lhs][*it] = i;
		}
	}
	return m;
}

static inline vector<int> initQ() {
	vector<int> q;
	q.push_back(S);
	return q;
}

static void printError() {
	std::cout << "Syntax error\n";
	exit(0);
}

void parser() {
}

*/