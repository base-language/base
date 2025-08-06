#pragma once

#include <string>

enum class TokenTypeEnum {
    Identifier,
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