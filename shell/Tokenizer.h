#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <vector>
#include <string>
using namespace std;

class Tokenizer {
public:
    static vector<string> tokenize(const string& input);
};
#endif // TOKENIZER_H
