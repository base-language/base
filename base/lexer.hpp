#pragma once
#include <string>
#include <unordered_set>

enum class TokenTypeEnum {
    Identifier,
    Keyword,
    Number,
    String,
    TemplateLiteral,
    Symbol,
    EndOfFile
};

struct Token {
    TokenTypeEnum type;
    std::string value;
    int line;
};

class Lexer {
public:
    Lexer(const std::string &src);
    Token nextToken();
    Token peekToken() const;

private:
    std::string source;
    size_t pos = 0;
    int line = 1;

    static const std::unordered_set<std::string> keywords;

    char peekChar() const;
    char peekNextChar() const;
    char advanceChar();
    void skipWhitespaceAndComments();

    Token readIdentifierOrKeyword();
    Token readNumber();
    Token readString();
    Token readTemplateLiteral();
    Token readSymbol();
};