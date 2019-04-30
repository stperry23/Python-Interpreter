
#ifndef __PARSER_HPP
#define __PARSER_HPP

#include "Token.hpp"
#include "Tokenizer.hpp"
#include "SymTab.hpp"
#include "ArithExpr.hpp"
#include "Statements.hpp"
//#include "StringNode.hpp"

#include<vector>
#include<iostream>
#include<map>


class Parser {
public:
    Parser(Tokenizer &tokenizer) : tokenizer{tokenizer} {}

    Statements *statements();
    Statements *suite();

    AssignmentStatement *assignStatement();
    PrintStatement *printStatement();
    ForStatement *forStatement();
    IfStatement *ifStatement();

    std::vector<ExprNode*> testList();
    std::vector<ExprNode*> arrayInit();

    // ArrayOps *arrayOps();
    // ArrayLen *arrayLen();

    ExprNode *test();
    ExprNode *or_test();
    ExprNode *and_test();
    ExprNode *not_test();

    ExprNode *rel_expr();
    ExprNode *rel_term();
    ExprNode *rel_primary();

    ExprNode *expr();
    ExprNode *term();
    ExprNode *factor();
    ExprNode *primary();

    std::string id();

private:
    Tokenizer &tokenizer;

    void die(std::string where, std::string message, Token &token);

};

#endif
