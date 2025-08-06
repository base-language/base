#pragma once

#include <string>
#include <vector>
#include <unordered_set>

#include "token.hpp"

class Lexer {
public:
    Lexer(const std::string& src);
    Token nextToken();
    Token peekToken() const;

private:
    std::string source;
    size_t pos = 0;

    int line = 1;
    char peekChar() const;
    char advanceChar();
    void skipWhitespaceAndComments();
    char peekNextChar() const;
    
    Token readSymbol();
    Token readIdentifierOrKeyword();
    Token readNumber();
    Token readString();
    Token makeSymbolToken(std::string symbol);

    static const std::unordered_set<std::string> keywords;
};
