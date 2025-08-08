#include "parser.hpp"
#include <stdexcept>
#include <sstream>
#include <unordered_map>

Parser::Parser(Lexer &lex) : lexer(lex) { advance(); }

void Parser::advance() { currentToken = lexer.nextToken(); }

bool Parser::match(const std::string &expected)
{
    if (check(expected))
    {
        advance();
        return true;
    }
    return false;
}
bool Parser::check(const std::string &expected) const
{
    if (currentToken.type == TokenTypeEnum::EndOfFile)
        return false;
    return currentToken.value == expected;
}
bool Parser::checkType(TokenTypeEnum expectedType) const
{
    return currentToken.type == expectedType;
}
Token Parser::consume(const std::string &expected, const std::string &errorMsg)
{
    if (check(expected))
    {
        Token token = currentToken;
        advance();
        return token;
    }
    throw std::runtime_error("Parse error at line " + std::to_string(currentToken.line) + ": " + errorMsg);
}
Token Parser::consumeType(TokenTypeEnum expectedType, const std::string &errorMsg)
{
    if (checkType(expectedType))
    {
        Token token = currentToken;
        advance();
        return token;
    }
    throw std::runtime_error("Parse error at line " + std::to_string(currentToken.line) + ": " + errorMsg);
}

std::unique_ptr<Program> Parser::parse() { return parseProgram(); }

std::unique_ptr<Program> Parser::parseProgram()
{
    std::vector<std::unique_ptr<ASTNode>> statements;
    while (currentToken.type != TokenTypeEnum::EndOfFile)
    {
        auto stmt = parseStatement();
        if (stmt)
            statements.push_back(std::move(stmt));
    }
    return std::make_unique<Program>(std::move(statements));
}

std::unique_ptr<ASTNode> Parser::parseStatement()
{
    if (currentToken.value == "let" || currentToken.value == "const")
        return parseVariableDeclaration();
    if (currentToken.value == "function")
        return parseFunctionDeclaration();
    if (currentToken.value == "print")
    {
        // Built-in print statement
        int line = currentToken.line;
        advance();
        consume("(", "Expected '(' after 'print'");
        auto arg = parseExpression();
        consume(")", "Expected ')' after print argument");
        consume(";", "Expected ';' after print statement");
        std::vector<std::unique_ptr<ASTNode>> args;
        args.push_back(std::move(arg));
        auto callee = std::make_unique<IdentifierExpression>("print", line);
        return std::make_unique<ExpressionStatement>(
            std::make_unique<CallExpression>(std::move(callee), std::move(args), line),
            line);
    }
    if (currentToken.value == "return")
    {
        int line = currentToken.line;
        advance();
        std::unique_ptr<ASTNode> arg = nullptr;
        // void fonksiyonlarda return; olabilir, diğerlerinde return <expr>;
        if (!check(";"))
        {
            arg = parseExpression();
        }
        consume(";", "Expected ';' after return statement");
        return std::make_unique<ReturnStatement>(std::move(arg), line);
    }
    if (check("{"))
        return parseBlockStatement();
    return parseExpressionStatement();
}

std::unique_ptr<VariableDeclaration> Parser::parseVariableDeclaration()
{
    std::string kind = currentToken.value;
    advance();
    std::vector<VariableDeclarator> declarations;
    do
    {
        std::string typeAnnotation;
        if (currentToken.type == TokenTypeEnum::Keyword && (currentToken.value == "number" || currentToken.value == "string"))
        {
            typeAnnotation = currentToken.value;
            advance();
        }
        else
            throw std::runtime_error("Parse error at line " + std::to_string(currentToken.line) + ": Expected type annotation, got '" + currentToken.value + "'");
        if (currentToken.type != TokenTypeEnum::Identifier)
            throw std::runtime_error("Parse error at line " + std::to_string(currentToken.line) + ": Expected identifier, got '" + currentToken.value + "'");
        std::string name = currentToken.value;
        advance();
        consume("=", "Assignment is required in variable declaration");
        auto init = parseExpression();
        declarations.emplace_back(name, std::move(init), typeAnnotation);
    } while (match(","));
    consume(";", "Expected ';' after variable declaration");
    return std::make_unique<VariableDeclaration>(kind, std::move(declarations), currentToken.line);
}

std::unique_ptr<FunctionDeclaration> Parser::parseFunctionDeclaration()
{
    advance();
    std::string returnType;
    if (currentToken.type == TokenTypeEnum::Keyword && (currentToken.value == "number" || currentToken.value == "string" || currentToken.value == "void"))
    {
        returnType = currentToken.value;
        advance();
    }
    else
        throw std::runtime_error("Parse error at line " + std::to_string(currentToken.line) + ": Expected return type, got '" + currentToken.value + "'");
    if (currentToken.type != TokenTypeEnum::Identifier)
        throw std::runtime_error("Parse error at line " + std::to_string(currentToken.line) + ": Expected function name, got '" + currentToken.value + "'");
    std::string name = currentToken.value;
    advance();
    consume("(", "Expected '(' after function name");
    std::vector<Parameter> params = parseParameterList();
    consume(")", "Expected ')' after parameters");
    auto body = parseBlockStatement();
    return std::make_unique<FunctionDeclaration>(name, std::move(params), std::unique_ptr<BlockStatement>(static_cast<BlockStatement *>(body.release())), currentToken.line, returnType);
}

std::unique_ptr<BlockStatement> Parser::parseBlockStatement()
{
    int line = currentToken.line;
    consume("{", "Expected '{'");
    std::vector<std::unique_ptr<ASTNode>> statements;
    while (!check("}") && currentToken.type != TokenTypeEnum::EndOfFile)
    {
        auto stmt = parseStatement();
        if (stmt)
            statements.push_back(std::move(stmt));
    }
    consume("}", "Expected '}'");
    return std::make_unique<BlockStatement>(std::move(statements), line);
}

std::unique_ptr<ASTNode> Parser::parseExpressionStatement()
{
    int line = currentToken.line;
    auto expr = parseExpression();
    consume(";", "Expected ';' after expression");
    return std::make_unique<ExpressionStatement>(std::move(expr), line);
}

std::unique_ptr<ASTNode> Parser::parseExpression()
{
    return parseBinaryExpression();
}

std::unique_ptr<ASTNode> Parser::parseBinaryExpression(int minPrec)
{
    auto left = parsePrimaryExpression();
    while (currentToken.type == TokenTypeEnum::Symbol &&
           (currentToken.value == "+" || currentToken.value == "-" ||
            currentToken.value == "*" || currentToken.value == "/"))
    {
        std::string op = currentToken.value;
        int line = currentToken.line;
        advance();
        auto right = parsePrimaryExpression();
        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right), line);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parsePrimaryExpression()
{
    int line = currentToken.line;
    if (currentToken.type == TokenTypeEnum::Number)
    {
        double value = std::stod(currentToken.value);
        advance();
        return std::make_unique<LiteralExpression>(value, line);
    }
    if (currentToken.type == TokenTypeEnum::String || currentToken.type == TokenTypeEnum::TemplateLiteral)
    {
        std::string value = currentToken.value;
        advance();
        return std::make_unique<LiteralExpression>(value, line);
    }
    if (currentToken.type == TokenTypeEnum::Identifier)
    {
        std::string name = currentToken.value;
        advance();
        if (match("("))
            return parseCallExpression(std::make_unique<IdentifierExpression>(name, line));
        return std::make_unique<IdentifierExpression>(name, line);
    }
    if (match("("))
    {
        auto expr = parseExpression();
        consume(")", "Expected ')' after expression");
        return expr;
    }
    throw std::runtime_error("Parse error at line " + std::to_string(line) + ": Unexpected token '" + currentToken.value + "'");
}

std::unique_ptr<ASTNode> Parser::parseCallExpression(std::unique_ptr<ASTNode> callee)
{
    int line = currentToken.line;
    auto args = parseArgumentList();
    consume(")", "Expected ')' after arguments");
    return std::make_unique<CallExpression>(std::move(callee), std::move(args), line);
}

std::vector<std::unique_ptr<ASTNode>> Parser::parseArgumentList()
{
    std::vector<std::unique_ptr<ASTNode>> args;
    while (!check(")") && currentToken.type != TokenTypeEnum::EndOfFile)
    {
        args.push_back(parseExpression());
        if (!check(")"))
            consume(",", "Expected ',' or ')' in argument list");
    }
    return args;
}

std::vector<Parameter> Parser::parseParameterList()
{
    std::vector<Parameter> params;
    while (!check(")") && currentToken.type != TokenTypeEnum::EndOfFile)
    {
        std::string paramType;
        if (currentToken.type == TokenTypeEnum::Keyword && (currentToken.value == "number" || currentToken.value == "string"))
        {
            paramType = currentToken.value;
            advance();
        }
        else
            throw std::runtime_error("Parse error at line " + std::to_string(currentToken.line) + ": Expected parameter type, got '" + currentToken.value + "'");
        if (currentToken.type != TokenTypeEnum::Identifier)
            throw std::runtime_error("Parse error at line " + std::to_string(currentToken.line) + ": Expected parameter name, got '" + currentToken.value + "'");
        std::string name = currentToken.value;
        advance();
        params.emplace_back(name, paramType);
        if (!check(")"))
            consume(",", "Expected ',' or ')' in parameter list");
    }
    return params;
}