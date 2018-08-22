#include "tokens.h"

evl_tokens tokens_;

bool extract_tokens_from_line(string line, int line_no, list<evl_token> &tokens)
{
	for (size_t i = 0; i < line.size(); ) {
		//comment
		if (line[i] == '/')
		{
			++i;
			if ((i == line.size()) || (line[i] != '/'))
			{
				cerr << "LINE " << line_no << ": a single / is not allowed" << endl;
				return false;
			}
			break; // skip the rest of the line by exiting the loop
		}

		// spaces
		else if ((line[i] == ' ') || (line[i] == '\t')
			|| (line[i] == '\r') || (line[i] == '\n'))
		{
			++i; // skip this space character
		}

		//SINGLE
		else if ((line[i] == '(') || (line[i] == ')')
			|| (line[i] == '[') || (line[i] == ']')
			|| (line[i] == ':') || (line[i] == ';')
			|| (line[i] == ','))
		{
			evl_token token;
			token.line_no = line_no;
			token.type = evl_token::SINGLE;
			token.str = string(1, line[i]);
			tokens.push_back(token);
			++i;
		}

		//Name
		else if (((line[i] >= 'a') && (line[i] <= 'z'))       // a to z
			|| ((line[i] >= 'A') && (line[i] <= 'Z'))    // A to Z
			|| (line[i] == '_'))
		{
			evl_token token; token.line_no = line_no;
			token.type = evl_token::NAME;
			size_t name_begin = i;
			for (++i; i < line.size(); ++i)
			{
				if (!(((line[i] >= 'a') && (line[i] <= 'z'))
					|| ((line[i] >= 'A') && (line[i] <= 'Z'))
					|| ((line[i] >= '0') && (line[i] <= '9'))
					|| (line[i] == '_') || (line[i] == '$')))
				{
					break; // [name_begin, i) is the range for the token
				}
			}
			token.str = line.substr(name_begin, i - name_begin);
			tokens.push_back(token);
		}

		//NUMBER
		else if ((line[i] >= '0') && (line[i] <= '9'))
		{
			evl_token token; token.line_no = line_no;
			token.type = evl_token::NUMBER;
			size_t number_begin = i;
			for (++i; i < line.size(); ++i)
			{
				if ((line[i]<'0') || (line[i]>'9'))
				{
					break; // [name_begin, i) is the range for the token
				}
			}
			token.str = line.substr(number_begin, i - number_begin);
			tokens.push_back(token);
		}

		else
		{
			cerr << "LINE " << line_no << ": invalid character" << endl;
			return false;
		}
	}
	return true; // nothing left
}

bool extract_tokens_from_file(string file_name, list<evl_token> &tokens)
{
	ifstream input_file(file_name.c_str());
	if (!input_file) {
		cerr << "I can't read " << file_name << "." << endl;
		return false;
	}

	string line;

	tokens.clear(); // be defensive, make it empty 

	for (int line_no = 1; getline(input_file, line); ++line_no)
	{
		if (!extract_tokens_from_line(line, line_no, tokens)) return false;
	}
	return true;
}

void display_tokens(ostream &out, list<evl_token> &tokens)
{
	for (evl_tokens::iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		if (it->type == evl_token::SINGLE)
		{
			out << "SINGLE " << it->str << endl;
		}
		else if (it->type == evl_token::NAME)
		{
			out << "NAME " << it->str << endl;
		}
		else if (it->type == evl_token::NUMBER)
		{
			out << "NUMBER " << it->str << endl;
		}
		else {
			cerr << "token type error" << endl;
		}
	}
}

bool store_tokens_to_file(string file_name, list<evl_token> &tokens) {
	ofstream output_file(file_name.c_str());
	if (!output_file)
	{
		cerr << "I can't write " << file_name << ".tokens ." << endl;
		return false;
	}

	display_tokens(output_file, tokens);

	return true;
}

bool parse_tokens_from_file(string evl_file, evl_tokens &tokens) {
	tokens.clear();

	if (!extract_tokens_from_file(evl_file, tokens))
	{
		return false;
	}

#ifdef TOKENS
	if (!store_tokens_to_file(evl_file + ".tokens", tokens))
	{
		return false;
	}
#endif

	return true;
}