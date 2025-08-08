#pragma once
#include <string>
#include <vector>
#include <memory>
#include "lexer.hpp"

// AST Base
struct ASTNode
{
    virtual ~ASTNode() = default;
};

struct Program : ASTNode
{
    std::vector<std::unique_ptr<ASTNode>> body;
    Program(std::vector<std::unique_ptr<ASTNode>> b) : body(std::move(b)) {}
};

struct LiteralExpression : ASTNode
{
    std::string strValue;
    double numValue{};
    bool isString = false, isNumber = false;
    LiteralExpression(const std::string &v, int) : strValue(v), isString(true) {}
    LiteralExpression(double v, int) : numValue(v), isNumber(true) {}
};

struct IdentifierExpression : ASTNode
{
    std::string name;
    IdentifierExpression(const std::string &n, int) : name(n) {}
};

struct VariableDeclarator
{
    std::string name;
    std::unique_ptr<ASTNode> init;
    std::string type;
    VariableDeclarator(std::string n, std::unique_ptr<ASTNode> i, std::string t)
        : name(std::move(n)), init(std::move(i)), type(std::move(t)) {}
};

struct VariableDeclaration : ASTNode
{
    std::string kind;
    std::vector<VariableDeclarator> declarations;
    int line;
    VariableDeclaration(std::string k, std::vector<VariableDeclarator> d, int l)
        : kind(std::move(k)), declarations(std::move(d)), line(l) {}
};

struct Parameter
{
    std::string name;
    std::string type;
    Parameter(std::string n, std::string t) : name(std::move(n)), type(std::move(t)) {}
};

struct BlockStatement;
struct FunctionDeclaration : ASTNode
{
    std::string name;
    std::vector<Parameter> params;
    std::unique_ptr<BlockStatement> body;
    int line;
    std::string returnType;
    FunctionDeclaration(std::string n, std::vector<Parameter> p, std::unique_ptr<BlockStatement> b, int l, std::string rt)
        : name(std::move(n)), params(std::move(p)), body(std::move(b)), line(l), returnType(std::move(rt)) {}
};

struct BlockStatement : ASTNode
{
    std::vector<std::unique_ptr<ASTNode>> body;
    int line;
    BlockStatement(std::vector<std::unique_ptr<ASTNode>> b, int l) : body(std::move(b)), line(l) {}
};

struct ReturnStatement : ASTNode
{
    std::unique_ptr<ASTNode> argument;
    int line;
    ReturnStatement(std::unique_ptr<ASTNode> arg, int l)
        : argument(std::move(arg)), line(l) {}
};

struct ExpressionStatement : ASTNode
{
    std::unique_ptr<ASTNode> expression;
    int line;
    ExpressionStatement(std::unique_ptr<ASTNode> e, int l) : expression(std::move(e)), line(l) {}
};

struct BinaryExpression : ASTNode
{
    std::unique_ptr<ASTNode> left;
    std::string op;
    std::unique_ptr<ASTNode> right;
    int line;
    BinaryExpression(std::unique_ptr<ASTNode> l, std::string o, std::unique_ptr<ASTNode> r, int li)
        : left(std::move(l)), op(std::move(o)), right(std::move(r)), line(li) {}
};

struct CallExpression : ASTNode
{
    std::unique_ptr<ASTNode> callee;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    int line;
    CallExpression(std::unique_ptr<ASTNode> c, std::vector<std::unique_ptr<ASTNode>> a, int l)
        : callee(std::move(c)), arguments(std::move(a)), line(l) {}
};

class Parser
{
public:
    Parser(Lexer &lex);
    std::unique_ptr<Program> parse();

private:
    Lexer &lexer;
    Token currentToken;

    void advance();
    bool match(const std::string &expected);
    bool check(const std::string &expected) const;
    bool checkType(TokenTypeEnum expectedType) const;
    Token consume(const std::string &expected, const std::string &errorMsg);
    Token consumeType(TokenTypeEnum expectedType, const std::string &errorMsg);

    std::unique_ptr<Program> parseProgram();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<VariableDeclaration> parseVariableDeclaration();
    std::unique_ptr<FunctionDeclaration> parseFunctionDeclaration();
    std::unique_ptr<BlockStatement> parseBlockStatement();
    std::unique_ptr<ASTNode> parseExpressionStatement();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseBinaryExpression(int minPrec = 0);
    std::unique_ptr<ASTNode> parsePrimaryExpression();
    std::unique_ptr<ASTNode> parseCallExpression(std::unique_ptr<ASTNode> callee);
    std::vector<std::unique_ptr<ASTNode>> parseArgumentList();
    std::vector<Parameter> parseParameterList();
};