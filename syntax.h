#pragma once

//#define SYNTAX

#ifndef SYNTAX_H
#define SYNTAX_H

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include "tokens.h"

using namespace std;

extern string module_name;

struct evl_statement
{
	enum statement_type
	{
		MODULE, WIRE, COMPONENT, ENDMODULE
	};

	statement_type type;
	evl_tokens tokens;

};
typedef list<evl_statement> evl_statements;

extern evl_statements statements_;

struct evl_wire
{
	string name;
	int width;
};	//	struct evl_wire
typedef list<evl_wire> evl_wires;

extern evl_wires wires;

struct evl_pin
{
	string name;
	int bus_lsb;
	int bus_msb;
};
typedef list<evl_pin> evl_pins;

struct evl_component
{
	string type;
	string name;
	evl_pins pin;
};	
typedef list<evl_component> evl_components;

extern evl_components comps;

typedef map<string, int> evl_wires_table;

extern evl_wires_table wires_table;

bool group_tokens_into_statements(evl_statements &statements, evl_tokens &tokens, string &module_name);

bool process_wire_statement(evl_wires &wires, evl_statement &s);

bool process_component_statement(evl_component &comp, evl_statement &s);

void display_statements(string file_name, evl_statements &statements, evl_wires &wires, evl_components &comps);

bool store_statements_to_file(string file_name, evl_statements statements, evl_wires &wires, evl_components &comps);

bool parse_statements_from_file(string evl_file, evl_wires &wires, evl_components &comps);

#endif

