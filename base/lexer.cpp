#include "lexer.hpp"

#include <iostream>
#include <cctype>

const std::unordered_set<std::string> Lexer::keywords = {
    // Variable and function declarations
    "let", "const", "var", "function", "return",

    // Types and type descriptions
    "number", "string", "boolean", "void", "any", "unknown", "never", "object", "array", "tuple", "enum", "interface", "type",

    // Control structures
    "if", "else", "switch", "case", "default",
    "for", "while", "do", "break", "continue",
    "try", "catch", "finally", "throw",

    // Logic and other expressions
    "true", "false", "null", "undefined", "this", "new", "delete", "typeof", "instanceof", "in", "await", "async", "yield",

    // Diğer önemli kelimeler
    "class", "extends", "implements", "export", "import", "from", "as", "namespace", "public", "private", "protected", "readonly", "static",

    // Built-in functions
    "print"};

Lexer::Lexer(const std::string &src) : source(src) {}

char Lexer::peekChar() const
{
    if (pos >= source.size())
        return '\0';
    return source[pos];
}
char Lexer::peekNextChar() const
{
    if (pos + 1 >= source.size())
        return '\0';
    return source[pos + 1];
}
char Lexer::advanceChar()
{
    char c = peekChar();
    if (c == '\n')
        line++;
    pos++;
    return c;
}
void Lexer::skipWhitespaceAndComments()
{
    while (true)
    {
        char c = peekChar();
        // Whitespace
        if (isspace(c))
        {
            advanceChar();
            continue;
        }
        if (c == '/' && peekNextChar() == '/')
        {
            advanceChar(); // '/'
            advanceChar(); // '/'
            while (peekChar() != '\n' && peekChar() != '\0')
            {
                advanceChar();
            }
            continue;
        }
        if (c == '/' && peekNextChar() == '*')
        {
            advanceChar(); // '/'
            advanceChar(); // '*'
            while (true)
            {
                char ch = peekChar();
                if (ch == '\0')
                {
                    std::cerr << "Lexer error: Unterminated block comment at line " << line << "\n";
                    return;
                }
                if (ch == '*' && peekNextChar() == '/')
                {
                    advanceChar(); // '*'
                    advanceChar(); // '/'
                    break;
                }
                advanceChar();
            }
            continue;
        }

        break;
    }
}
Token Lexer::readIdentifierOrKeyword()
{
    std::string value;
    char c = peekChar();
    // Identifier: starts with letter or _, then letters, digits, _
    while (isalnum(c) || c == '_')
    {
        value += advanceChar();
        c = peekChar();
    }
    if (keywords.count(value))
    {
        return {TokenTypeEnum::Keyword, value, line};
    }
    return {TokenTypeEnum::Identifier, value, line};
}
Token Lexer::readNumber()
{
    std::string value;
    bool hasDot = false;
    bool hasExp = false;
    char c = peekChar();
    while (true)
    {
        if (isdigit(c))
        {
            value += advanceChar();
        }
        else if (c == '.' && !hasDot)
        {
            hasDot = true;
            value += advanceChar();
        }
        else if ((c == 'e' || c == 'E') && !hasExp)
        {
            hasExp = true;
            value += advanceChar();
            // exponent can be followed by optional + or -
            char next = peekChar();
            if (next == '+' || next == '-')
            {
                value += advanceChar();
            }
        }
        else
        {
            break;
        }
        c = peekChar();
    }
    return {TokenTypeEnum::Number, value, line};
}
Token Lexer::readString()
{
    advanceChar(); // skip opening quote
    std::string value;
    while (true)
    {
        char c = peekChar();
        if (c == '\0')
        {
            std::cerr << "Lexer error: Unterminated string at line " << line << "\n";
            break;
        }
        if (c == '"')
        {
            advanceChar(); // skip closing quote
            break;
        }
        if (c == '\\')
        {
            advanceChar();
            char esc = peekChar();
            switch (esc)
            {
            case 'n':
                value += '\n';
                break;
            case 't':
                value += '\t';
                break;
            case 'r':
                value += '\r';
                break;
            case '"':
                value += '"';
                break;
            case '\\':
                value += '\\';
                break;
            default:
                std::cerr << "Lexer warning: Unknown escape sequence \\" << esc << " at line " << line << "\n";
                value += esc;
            }
            advanceChar();
        }
        else
        {
            value += advanceChar();
        }
    }
    return {TokenTypeEnum::String, value, line};
}
Token Lexer::readSymbol()
{
    std::string twoCharSym;
    twoCharSym += peekChar();
    twoCharSym += peekNextChar();
    static const std::unordered_set<std::string> multiCharSymbols = {
        "==", "!=", "<=", ">=", "&&", "||", "+=", "-=", "*=", "/=", "%=", "++", "--", ">>", "<<"};
    if (pos + 1 < source.size() && multiCharSymbols.count(twoCharSym))
    {
        pos += 2;
        return {TokenTypeEnum::Symbol, twoCharSym, line};
    }
    else
    {
        char c = advanceChar();
        return {TokenTypeEnum::Symbol, std::string(1, c), line};
    }
}
Token Lexer::nextToken()
{
    skipWhitespaceAndComments();
    char c = peekChar();
    if (c == '\0')
        return {TokenTypeEnum::EndOfFile, "", line};
    if (isalpha(c) || c == '_')
        return readIdentifierOrKeyword();
    if (isdigit(c))
        return readNumber();
    if (c == '"')
        return readString();
    if (c == '-')
        return readSymbol();
    return readSymbol();
}
Token Lexer::peekToken() const
{
    size_t savedPos = pos;
    int savedLine = line;
    Lexer temp = *this;
    Token t = temp.nextToken();
    return t;
}