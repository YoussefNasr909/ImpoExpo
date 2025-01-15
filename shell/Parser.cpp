#include "Parser.h"
using namespace std;

Command Parser::parse(const std::vector<std::string>& tokens) {
    Command cmd;
    if (!tokens.empty()) {
        cmd.name = tokens[0];
        for (size_t i = 1; i < tokens.size(); ++i) {
            cmd.arguments.push_back(tokens[i]);
        }
    }
    return cmd;
}
