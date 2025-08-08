#pragma once

#include <string>

enum class TokenTypeEnum {
    Identifier,
    TemplateLiteral,
    Number,
    String,
    Keyword,
    Symbol,
    EndOfFile
};
struct Token {
    TokenTypeEnum type;
    std::string value;
    int line;
};