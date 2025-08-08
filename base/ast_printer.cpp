#include "ast_printer.hpp"

void ASTPrinter::print(const ASTNode *node, int indent)
{
    if (!node)
        return;
    if (auto p = dynamic_cast<const Program *>(node))
        printProgram(p, indent);
    else if (auto p = dynamic_cast<const LiteralExpression *>(node))
        printLiteral(p, indent);
    else if (auto p = dynamic_cast<const IdentifierExpression *>(node))
        printIdentifier(p, indent);
    else if (auto p = dynamic_cast<const VariableDeclaration *>(node))
        printVarDecl(p, indent);
    else if (auto p = dynamic_cast<const FunctionDeclaration *>(node))
        printFuncDecl(p, indent);
    else if (auto p = dynamic_cast<const BlockStatement *>(node))
        printBlock(p, indent);
    else if (auto p = dynamic_cast<const ReturnStatement *>(node))
        printReturn(p, indent);
    else if (auto p = dynamic_cast<const ExpressionStatement *>(node))
        printExprStmt(p, indent);
    else if (auto p = dynamic_cast<const BinaryExpression *>(node))
        printBinary(p, indent);
    else if (auto p = dynamic_cast<const CallExpression *>(node))
        printCall(p, indent);
    else
        printIndent(indent), std::cout << "(Unknown AST node)\n";
}

void ASTPrinter::printIndent(int indent)
{
    for (int i = 0; i < indent; ++i)
        std::cout << "  ";
}

void ASTPrinter::printProgram(const Program *node, int indent)
{
    printIndent(indent);
    std::cout << "Program\n";
    for (const auto &stmt : node->body)
        print(stmt.get(), indent + 1);
}

void ASTPrinter::printLiteral(const LiteralExpression *node, int indent)
{
    printIndent(indent);
    if (node->isString)
        std::cout << "StringLiteral: \"" << node->strValue << "\"\n";
    else if (node->isNumber)
        std::cout << "NumberLiteral: " << node->numValue << "\n";
}

void ASTPrinter::printIdentifier(const IdentifierExpression *node, int indent)
{
    printIndent(indent);
    std::cout << "Identifier: " << node->name << "\n";
}

void ASTPrinter::printVarDecl(const VariableDeclaration *node, int indent)
{
    printIndent(indent);
    std::cout << node->kind << " VariableDeclaration\n";
    for (const auto &decl : node->declarations)
    {
        printIndent(indent + 1);
        std::cout << decl.type << " " << decl.name << " =\n";
        print(decl.init.get(), indent + 2);
    }
}

void ASTPrinter::printFuncDecl(const FunctionDeclaration *node, int indent)
{
    printIndent(indent);
    std::cout << "FunctionDeclaration: " << node->name << " -> " << node->returnType << "\n";
    printIndent(indent + 1);
    std::cout << "Params:";
    for (const auto &param : node->params)
        std::cout << " " << param.type << " " << param.name;
    std::cout << "\n";
    print(node->body.get(), indent + 1);
}

void ASTPrinter::printBlock(const BlockStatement *node, int indent)
{
    printIndent(indent);
    std::cout << "Block\n";
    for (const auto &stmt : node->body)
        print(stmt.get(), indent + 1);
}

void ASTPrinter::printReturn(const ReturnStatement *node, int indent)
{
    printIndent(indent);
    std::cout << "ReturnStatement\n";
    if (node->argument)
        print(node->argument.get(), indent + 1);
}

void ASTPrinter::printExprStmt(const ExpressionStatement *node, int indent)
{
    printIndent(indent);
    std::cout << "ExpressionStatement\n";
    print(node->expression.get(), indent + 1);
}

void ASTPrinter::printBinary(const BinaryExpression *node, int indent)
{
    printIndent(indent);
    std::cout << "BinaryExpression: " << node->op << "\n";
    print(node->left.get(), indent + 1);
    print(node->right.get(), indent + 1);
}

void ASTPrinter::printCall(const CallExpression *node, int indent)
{
    printIndent(indent);
    std::cout << "CallExpression\n";
    print(node->callee.get(), indent + 1);
    for (const auto &arg : node->arguments)
        print(arg.get(), indent + 2);
}