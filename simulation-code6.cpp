#include<iostream>
#include<fstream>
#include<sstream>
#include<cassert>
#include<string>
#include<vector>
#include<list>
#include<map>
#include<string.h>
#include<cstdlib>
#include<iomanip>

#include"tokens.h"
#include"syntax.h"
#include"netlist.h"

using namespace std;


int main(int argc, char *argv[])
{

	string evl_file;

	//validate arguments
	if (argc < 2) return -1;

	evl_file = argv[1];
#ifdef TOKENS
	parse_tokens_from_file(evl_file, tokens_);
#endif
#ifdef SYNTAX
	parse_statements_from_file(evl_file, wires, comps);
#endif
#ifdef NETLIST
	parse_netlists_from_file(evl_file, wires, comps);
#endif

	return 0;
}
