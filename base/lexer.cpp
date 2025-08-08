#include "lexer.hpp"
#include <iostream>
#include <cctype>
#include <stdexcept>

const std::unordered_set<std::string> Lexer::keywords = {
    "let", "const", "function", "return",
    "number", "string", "void",
    "if", "else", "for", "while", "break", "continue", "print"
};

Lexer::Lexer(const std::string &src) : source(src) {}

char Lexer::peekChar() const {
    if (pos >= source.size()) return '\0';
    return source[pos];
}
char Lexer::peekNextChar() const {
    if (pos + 1 >= source.size()) return '\0';
    return source[pos + 1];
}
char Lexer::advanceChar() {
    char c = peekChar();
    pos++;
    if (c == '\n') line++;
    return c;
}
void Lexer::skipWhitespaceAndComments() {
    while (true) {
        char c = peekChar();
        if (c == '\0') break;
        if (isspace(static_cast<unsigned char>(c))) {
            advanceChar();
            continue;
        }
        if (c == '/' && peekNextChar() == '/') {
            advanceChar(); advanceChar();
            while (peekChar() != '\n' && peekChar() != '\0') advanceChar();
            continue;
        }
        if (c == '/' && peekNextChar() == '*') {
            advanceChar(); advanceChar();
            while (true) {
                char ch = peekChar();
                if (ch == '\0') return;
                if (ch == '*' && peekNextChar() == '/') {
                    advanceChar(); advanceChar();
                    break;
                }
                advanceChar();
            }
            continue;
        }
        break;
    }
}
Token Lexer::readIdentifierOrKeyword() {
    std::string value;
    char c = peekChar();
    while (isalnum(static_cast<unsigned char>(c)) || c == '_') {
        value += advanceChar();
        c = peekChar();
    }
    if (keywords.count(value))
        return {TokenTypeEnum::Keyword, value, line};
    return {TokenTypeEnum::Identifier, value, line};
}
Token Lexer::readNumber() {
    std::string value;
    bool hasDot = false;
    char c = peekChar();
    while (isdigit(static_cast<unsigned char>(c)) || (c == '.' && !hasDot)) {
        if (c == '.') hasDot = true;
        value += advanceChar();
        c = peekChar();
    }
    return {TokenTypeEnum::Number, value, line};
}
Token Lexer::readString() {
    char opener = peekChar();
    if (opener != '"') throw std::runtime_error("readString called on non-\" at line " + std::to_string(line));
    advanceChar();
    std::string value;
    while (true) {
        char c = peekChar();
        if (c == '\0') break;
        if (c == '\n') throw std::runtime_error("Multi-line string not allowed with double quotes at line " + std::to_string(line));
        if (c == '"') { advanceChar(); break; }
        if (c == '\\') {
            advanceChar();
            char esc = peekChar();
            switch (esc) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '"': value += '"'; break;
                case '\\': value += '\\'; break;
                default: value += esc;
            }
            advanceChar();
        } else {
            value += advanceChar();
        }
    }
    return {TokenTypeEnum::String, value, line};
}
Token Lexer::readTemplateLiteral() {
    advanceChar(); // skip `
    std::string value;
    int startLine = line;
    while (true) {
        char c = peekChar();
        if (c == '\0') break;
        if (c == '`') { advanceChar(); break; }
        if (c == '\\') {
            advanceChar();
            char esc = peekChar();
            if (esc == 'n') value += '\n';
            else if (esc == 't') value += '\t';
            else if (esc == 'r') value += '\r';
            else value += esc;
            advanceChar();
            continue;
        }
        value += advanceChar();
    }
    return {TokenTypeEnum::TemplateLiteral, value, startLine};
}
Token Lexer::readSymbol() {
    std::string two;
    char a = peekChar();
    char b = peekNextChar();
    two += a; two += b;
    static const std::unordered_set<std::string> twoChar = {
        "==", "!=", "<=", ">=", "+=", "-=", "*=", "/=", "++", "--"
    };
    if (twoChar.count(two)) {
        pos += 2;
        return {TokenTypeEnum::Symbol, two, line};
    }
    char c = advanceChar();
    return {TokenTypeEnum::Symbol, std::string(1, c), line};
}
Token Lexer::nextToken() {
    skipWhitespaceAndComments();
    char c = peekChar();
    if (c == '\0') return {TokenTypeEnum::EndOfFile, "", line};
    if (isalpha(static_cast<unsigned char>(c)) || c == '_')
        return readIdentifierOrKeyword();
    if (isdigit(static_cast<unsigned char>(c)))
        return readNumber();
    if (c == '"')
        return readString();
    if (c == '`')
        return readTemplateLiteral();
    if (c == '\'')
        throw std::runtime_error("Single-quote strings not allowed in BASE at line " + std::to_string(line));
    return readSymbol();
}
Token Lexer::peekToken() const {
    Lexer temp = *this;
    return temp.nextToken();
}