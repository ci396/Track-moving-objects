#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class evl_token
{
public: 
	enum token_type {NAME, NUMBER, SINGLE};
    token_type type;
    string str;
    int line_no;
};

class evl_token_name : public evl_token
{
public:
	bool isModule()
	{
		return ("module" == str);
	}
	
	evl_token_name()
	{
		type = evl_token::NAME;
	}
};

bool extract_tokens_from_line(string line, int line_no, vector<evl_token> &tokens)
{ // use reference to modify it
	for (size_t i = 0; i < line.size();)
	{
		// comments
		if (line[i] == '/')
		{
			++i;
			if ((i == line.size()) || (line[i] != '/'))
			{
				cerr << "LINE " << line_no
					<< ": a single / is not allowed" << endl;
				return false;
			}
			break; // skip the rest of the line by exiting the loop
		}

		// spaces
		if ((line[i] == ' ') || (line[i] == '\t')
			|| (line[i] == '\r') || (line[i] == '\n'))
		{
			++i; // skip this space character
			continue; // skip the rest of the iteration
		}

		// SINGLE
		if ((line[i] == '(') || (line[i] == ')')
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
			continue;
		}

		//NUMBER
		if ((line[i] >= '0') && (line[i] <= '9'))
		{
			size_t number_begin = i;
			for (++i; i < line.size(); ++i)
			{
				if (!((line[i] >= '0') && (line[i] <= '9')))
				{
					break; // [name_begin, i) is the range for the token
				}
			}
			evl_token token;
			token.line_no = line_no;
			token.type = evl_token::NUMBER;
			token.str = line.substr(number_begin, i - number_begin);
			tokens.push_back(token);
			continue;// skip the rest of the iteration
		}

		// NAME
		if (((line[i] >= 'a') && (line[i] <= 'z'))       // a to z
			|| ((line[i] >= 'A') && (line[i] <= 'Z'))    // A to Z
			|| (line[i] == '_'))
		{
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
			evl_token token;
			token.line_no = line_no;
			token.type = evl_token::NAME;
			token.str = line.substr(name_begin, i - name_begin);
			tokens.push_back(token);
			continue;
		}
		else
		{
			cerr << "LINE " << line_no << ": invalid character" << endl;
			return false;
		}
	}
	return true; // nothing left
}

bool extract_tokens_from_file(string file_name, vector<evl_token> &tokens)
{ // use reference to modify it
    ifstream input_file(file_name);
    if (!input_file) {
        cerr << "I cannot read " << file_name << "." << endl;
        return false;//return -4
    }

    tokens.clear(); // be defensive, make it empty
    
    string line;
    for (int line_no = 1; getline(input_file, line); ++line_no) {
        if (!extract_tokens_from_line(line, line_no, tokens)) {
            return false;//return -4
        }
    }
    return true;
}

void display_tokens(const vector<evl_token> &tokens)
{
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        if (tokens[i].type == evl_token::SINGLE)
        {
            cout << "SINGLE " << tokens[i].str << endl;
        }
        else if (tokens[i].type == evl_token::NAME)
        {
            cout << "NAME " << tokens[i].str << endl;
        }
        else
        { // must be NUMBER
            cout << "NUMBER " << tokens[i].str << endl;
        }
    }
}

void display_tokens(ostream &out, const vector<evl_token> &tokens)
{
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        if (tokens[i].type == evl_token::SINGLE)
        {
            out << "SINGLE " << tokens[i].str << endl;
        }
        else if (tokens[i].type == evl_token::NAME)
        {
			if ("module" == tokens[i].str) {
				out << "module ";
				i++;
				out << tokens[i].str << endl;
			}
            else if ("endmodule" == tokens[i].str) {
                i++;
            }
			else if("wire" == tokens[i].str){
				//
			}
            //out << "NAME " << tokens[i].str << endl;
        }
        else
        { // must be NUMBER
            out << "NUMBER " << tokens[i].str << endl;
        }
    }
}

bool store_tokens_to_file(string file_name, const vector<evl_token> &tokens)
{
    ofstream output_file(file_name.c_str());
	if (!output_file)// verify output_file is ready
	{
		cerr << "I cannnot write " << file_name << endl;
		return false;//return -5
	}

    display_tokens(output_file, tokens);
    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        return -1;
    }

    string evl_file = argv[1];
    vector<evl_token> tokens;

    if (!extract_tokens_from_file(evl_file, tokens))
    {
        return -4;
    }

    display_tokens(tokens); // why we need it?

    if (!store_tokens_to_file(evl_file+".syntax", tokens))
    {
        return -5;
    }

    
    return 0;
}
