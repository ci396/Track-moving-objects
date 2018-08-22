#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "You should provide a file name." << endl;
        return -1;
    }

    ifstream input_file(argv[1]);
    if (!input_file)
    {
        cerr << "I can't read " << argv[1] << "." << endl;
        return -2;
    }

    string output_file_name = string(argv[1])+".tokens";
    ofstream output_file(output_file_name);
    if (!output_file)
    {
        cerr << "I can't write " << argv[1] << ".tokens ." << endl;
        return -3;
    }

    string line;
    for (int line_no = 1; getline(input_file, line); ++line_no)
    {
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
                    return -4;
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
                output_file << "SINGLE " << line[i] << endl;
                ++i; // we consumed this character
                continue; // skip the rest of the iteration
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
                output_file << "NUMBER "
                    << line.substr(number_begin, i - number_begin) << endl;
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
                output_file << "NAME "
                    << line.substr(name_begin, i-name_begin) << endl;
            }
            else
            {
                cerr << "LINE " << line_no
                    << ": invalid character" << endl;
                return -5;
            }
        }
    }

    return 0;
}
