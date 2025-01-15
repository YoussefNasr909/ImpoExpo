#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <vector>
using namespace std;
struct Command {
    string name;                 // The command name (e.g., "rename")
    vector<string> arguments; // List of arguments (e.g., ["old file.txt", "new file.txt"])
};

class Parser {
public:
    static Command parse(const vector<string>& tokens);
};

#endif // PARSER_H
