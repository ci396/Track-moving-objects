#pragma once

//#define TOKENS

#ifndef TOKENS_H
#define TOKENS_H

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <sstream>
#include <assert.h>
#include <map>

using namespace std;

struct evl_token
{
	enum token_type
	{
		NAME, NUMBER, SINGLE
	};
	token_type type;
	string str;

	int line_no;
};
typedef list<evl_token> evl_tokens;

extern evl_tokens tokens_;

bool extract_tokens_from_line(string line, int line_no, list<evl_token> &tokens);

bool extract_tokens_from_file(string file_name, list<evl_token> &tokens);

void display_tokens(ostream &out, list<evl_token> &tokens);

bool store_tokens_to_file(string file_name, list<evl_token> &tokens);

bool parse_tokens_from_file(string evl_file, evl_tokens &tokens);

#endif
