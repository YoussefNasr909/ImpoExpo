#include "Tokenizer.h"
#include <sstream>
#include <cctype>
using namespace std;
vector<string> Tokenizer::tokenize(const string& input)
{
    vector<string> tokens;
    istringstream stream(input);
    string token;
    bool inQuotes = false;
    string currentToken;

    while (stream >> ws) { // Skip any leading whitespace
        char c = stream.peek();
        if (c == '\"') {
            // Handle quoted string
            stream.get(); // Consume the opening quote
            getline(stream, token, '\"'); // Read until closing quote
            tokens.push_back(token);
        }
        else {
            // Read the next word
            stream >> token;
            tokens.push_back(token);
        }
    }

    return tokens;
}
