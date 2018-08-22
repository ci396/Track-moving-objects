#pragma once

#define NETLIST

#ifndef GUARD_NETLIST_H
#define GUARD_NETLIST_H

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <sstream>
#include <assert.h>
#include <map>
#include "syntax.h"

using namespace std;

class netlist;
class gate;
class net;
class pin;

//variable declaration
extern netlist nl;

class net
{
public:
	string name; // e.g. "a", "s[0]" 
	char signal; // e.g. ’0’ or ’1’ 
	net(string name) : name(name) {}
	list<pin *> connections; // relationship "connect" 
	void append_pin(pin *p);
}; // class net
class pin
{
public:
	char dir_; // e.g. ’I’ or ’O’ 
	gate *gate_; // relationship "contain" 
	size_t index_; // attribute of "contain" n
	net *net_; // relationship "connect"
	vector<net *> bus_;
	bool create(gate *g, size_t index, const evl_pin &p, const map<string, net *> &nets_table);
}; // class pin
class gate
{
public:
	string name_; string type_; // e.g. "and", "or" 
	vector<pin *> pins_;
	bool create(evl_component &c, const map<string, net *> &nets_table,
		const evl_wires_table &wires_table);
	// relationship "contain" 
	bool create_pin(evl_pin &ep, size_t index, const map<string, net *>
		&nets_table, const evl_wires_table &wires_table);
}; // class gate
class netlist
{
public:
	list<gate *> gates_;
	list<net *> nets_;
	map<string, net *> nets_table_;
	void create_net(string net_name);
	bool create_nets(const evl_wires &wires);
	bool create_gate(evl_component &c, const evl_wires_table &wires_table);
	bool create_gates(evl_components &comps, const evl_wires_table &wires_table);
	bool create(const evl_wires &wires, evl_components &comps, const evl_wires_table &wires_table);
	void save(string file_name, string module_name);
}; // class netlist

bool make_wires_table(const evl_wires &wires, evl_wires_table &wires_table);

string make_net_name(string wire_name, int i);

bool parse_netlists_from_file(string evl_file, evl_wires &wires, evl_components &comps);

#endif