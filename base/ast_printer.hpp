#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "parser.hpp"

// AST pretty printer for debugging
class ASTPrinter
{
public:
    static void print(const ASTNode *node, int indent = 0);

private:
    static void printIndent(int indent);
    static void printProgram(const Program *node, int indent);
    static void printLiteral(const LiteralExpression *node, int indent);
    static void printIdentifier(const IdentifierExpression *node, int indent);
    static void printVarDecl(const VariableDeclaration *node, int indent);
    static void printFuncDecl(const FunctionDeclaration *node, int indent);
    static void printBlock(const BlockStatement *node, int indent);
    static void printReturn(const ReturnStatement *node, int indent);
    static void printExprStmt(const ExpressionStatement *node, int indent);
    static void printBinary(const BinaryExpression *node, int indent);
    static void printCall(const CallExpression *node, int indent);
};