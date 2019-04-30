
/*  The majority of the work is done by the class 'convert'.
    This class builds an expression tree using the input infix
    expression.  A post-order traversal of the expression tree 'dumps'
    it into an array in postfix form.  The iterator copies the token
    from this array to user's arrays.

*/

#include<vector>
#include<iostream>
#include<stack>

#include "Token.hpp"
#include "Parser.hpp"
#include "Statements.hpp"


// Parser functions

void Parser::die(std::string where, std::string message, Token &token) {
    std::cout << where << " " << message << std::endl;
    token.print();
    std::cout << std::endl;
    std::cout << "\nThe following is a list of tokens that have been identified up to this point.\n";
    tokenizer.printProcessedTokens();
    exit(1);
}
// need to change to handle assignmeent statements and array calls
Statements *Parser::statements() {
    // This function is called when we KNOW that we are about to parse
    // a series of assignment statements.


    Statements *stmts = new Statements();
    // make the function list to store functions
    FuncList *funcList = new FuncList();
    Token tok = tokenizer.getToken();
    while (tok.isName() || tok.eol()) {
        if(tok.isKeyword()){
            if (tok.getName() == "print"){
                tokenizer.ungetToken();
                PrintStatement *printStmt = printStatement();
                stmts->addStatement(printStmt);
                tok = tokenizer.getToken();
            }
            if (tok.getName() == "for"){
                tokenizer.ungetToken();
                ForStatement *forStmt = forStatement();
                stmts->addStatement(forStmt);
                tok = tokenizer.getToken();
            }
            if (tok.getName() == "if") {
                tokenizer.ungetToken();
                IfStatement *ifStmt = ifStatement();
                stmts->addStatement(ifStmt);
                tok = tokenizer.getToken();
            }
            if (tok.getName() == "def") {
                tokenizer.ungetToken();
                FuncStatement *func = funcStatement();
                funcList->addFunc(func->functionName(), func)
                tok = tokenizer.getToken();
            }
        }
        else if (tok.eol()) {
            tok = tokenizer.getToken();
            continue;
        }
        // change here for
        else {
            tokenizer.ungetToken();
            AssignmentStatement *assignStmt = assignStatement();
            stmts->addStatement(assignStmt);
            tok = tokenizer.getToken();
        }
    }
    tokenizer.ungetToken();
    return stmts;
}

AssignmentStatement *Parser::assignStatement() {
    Token varName = tokenizer.getToken();
    if (!varName.isName())
        die("Parser::assignStatement", "Expected a name token, instead got", varName);
    Token assignOp = tokenizer.getToken();
    if (!assignOp.isAssignmentOperator())
        die("Parser::assignStatement", "Expected an equal sign, instead got", assignOp);

    ExprNode *rightHandSideExpr = nullptr;
    std::vector<ExprNode*> arrayContent;
    Token openBrack = tokenizer.getToken();
    if (openBrack.isOpenBrack()) {
        tokenizer.ungetToken();
        arrayContent = arrayInit();
    }
    else
        rightHandSideExpr = test();
    Token endOfLine = tokenizer.getToken();
    if (!endOfLine.eol())
        die("Parser::assignStatement", "Expected an end of line character, instead got", endOfLine);

    return new AssignmentStatement(varName.getName(), rightHandSideExpr, arrayContent);
}



PrintStatement *Parser::printStatement() {
    Token printName = tokenizer.getToken();
    if (!printName.isKeyword())
        die("Parser::printStatement", "Expected print key word token, instead got", printName);

    std::vector<ExprNode*> list = testList();

    Token endOfLine = tokenizer.getToken();
    if (!endOfLine.eol())
        die("Parser::printStatement", "Expected end of line, instead got", endOfLine);

    return new PrintStatement(list);

}

ForStatement *Parser::forStatement() {
    // For Keyword
    Token forName = tokenizer.getToken();
    if(!forName.isKeyword())
        die("Parser::forStatement", "Expected for key word token, instead got", forName);

    // forVar
    Token forVar = tokenizer.getToken();
    if(!forVar.isName())
        die("Parser::forStatement", "Expected for loop variable, instead got", forVar);

    // For in keyword
    Token forInKey = tokenizer.getToken();
    if(!forInKey.isInKey())
        die("Parser::forStatement", "Expected in keyword, instead got", forInKey);

    // Range keyword
    Token rangeKeyword = tokenizer.getToken();
    if(!rangeKeyword.isRangeKey())
        die("Parser::forStatement", "Expected range keyword, instead got", rangeKeyword);

    // First parenthesis
    Token firstParen = tokenizer.getToken();
    if(!firstParen.isOpenParen())
        die("Parser::forStatement", "Expected open parenthesis, instead got", firstParen);

    // Call testlist to parse the conditions of the range function
    std::vector<ExprNode*> testlist = testList();
    forRange *r = new forRange(rangeKeyword);

    // Closing parenthesis
    Token secParen = tokenizer.getToken();
    if(!secParen.isCloseParen())
        die("Parser::forStatement", "Expected closing parenthesis, instead got", secParen);

    // For colon
    Token colon = tokenizer.getToken();
    if(!colon.isColon())
        die("Parser::forStatement", "Expected colon, instead got", colon);
    Statements *forstmts = suite();

    return new ForStatement(forVar.getName(), r, testlist, forstmts);

}

IfStatement *Parser::ifStatement() {
    // If keyword
    Token ifKey = tokenizer.getToken();
    if (ifKey.getName() != "if")
        die("Parser::IfStatment", "Expected if keyword, instead got", ifKey);

    // condition
    ExprNode *cond = test();

    // Colon
    Token colonKey = tokenizer.getToken();
    if (!colonKey.isColon())
        die("Parser::IfStatement", "Expected colon key, instead got", colonKey);

    // Statements
    Statements *ifstmts = suite();

    // elif

    Statements *elifStmts = nullptr;
    std::vector<ExprNode*> elifCond;
    std::vector<Statements*> elifList;
    Token elifKey = tokenizer.getToken();
    while(elifKey.isElifKey()) {
        // get elif cond
        ExprNode *elifcond = test();
        elifCond.push_back(elifcond);

        // check for colon
        Token elifColon = tokenizer.getToken();
        if (!elifColon.isColon())
            die("Parser::IfStatement elif", "Expected colon key, instead got", elifColon);
        // get elif statements
        elifStmts = suite();
        elifList.push_back(elifStmts);

        // get next token to see if there are more elifs
        elifKey = tokenizer.getToken();

    }
    // not an elif, putting token back
    tokenizer.ungetToken();

    Statements *elseStmts = nullptr;
    // else
    Token elseKey = tokenizer.getToken();
    if (!elseKey.isElseKey()) {
        std::cout << "Parser::ifStatement Else: No else key found, putting token back" << std::endl;
        tokenizer.ungetToken();
    }
    else {
        // colon check
        Token elseColon = tokenizer.getToken();
        if (!elseColon.isColon())
            die("Parser::IfStatement Else", "Expected colon key, instead got", elseColon);
        elseStmts = suite();

    }
    return new IfStatement(cond, ifstmts, elseStmts, elifList, elifCond);
}

FuncStatement *Parser::funcStatement() {
    // Def
    Token defTok = tokenizer.getToken();
    if(!defTok.getName() == "def")
        die("Parser::funcStatement", "Expected def for function, instead got", defTok);

    // Function name
    Token funcName = tokenizer.getToken();
    if(!funcName.isName())
        die("Parser::funcStatement", "Expected function name, instead got", funcName);

    // Open parenthesis
    Token openParen = tokenizer.getToken();
    if(!openParen.isOpenParen())
        die("Parser::funcStatement", "Expected open parenthesis, instead got", openParen);

    // Parameter list
    std::vector<std::string> paramList = parameter_list();

    // Close parenthesis
    Token closeParen = tokenizer.getToken();
    if(!closeParen.isCloseParen())
        die("Parser::funcStatement", "Expected closing parenthesis, instead got", closeParen);

    // Colon
    Token funcColon = tokenizer.getToken();
    if(!funcColon.isColon())
        die("Parser::funcStatement", "Expected colon, instead got", funcColon);

    // Function statements
    Statements *funcStatements = func_suite();

    // Got all we need for a funciont, now return new function statement
    return new FuncStatement(funcName.getName(), paramList, funcStatements);
}

Statements *Parser::func_suite() {
    // New Line/End of line tok
    Token newLine = tokenizer.getToken();
    if(!newLine.eol())
        die("Parser::func_suite", "Expected new line char, instead got", newLine);

    // Indent check
    Token indentTok = tokenizer.getToken();
    if(!indentTok.isIndent())
        die("Parser::func_suite", "Expected indent, instead got", indentTok);

    // Parse statements or return statement
    Statements *func_suiteStmts = statements();

    // Dedent
    Token dedentTok = tokenizer.getToken();
    if(!dedentTok.isDedent())
        die("Parser::func_suite", "Expected dedent, instead got", dedentTok);
    return func_suiteStmts;
}

Statements *Parser::suite() {
    // For end of line
    Token endOfLine = tokenizer.getToken();
    if(!endOfLine.eol())
        die("Parser::suite", "Expected end of line, instead got", endOfLine);

    // Statements of the for loop
    // Call suite
    Token id = tokenizer.getToken();
    if(!id.isIndent())
        die("Parser::suite", "Expected indent, instead got", id);
    Statements *suitestmts = statements();

    // Dedent
    Token dd = tokenizer.getToken();
    if(!dd.isDedent()) {
        die("Parser::suite", "Expected dedent, instead got", dd);
    }
    return suitestmts;

}

std::vector<ExprNode*> Parser::arrayInit() {
    // Check for open bracket
    Token openBracket = tokenizer.getToken();
    if(!openBracket.isOpenBrack())
        die("Parser::arrayInit", "Expected open brack, instead got", openBracket);
    // get array elements
    std::vector<ExprNode*> arrayContents = testList();

    // Check for close bracket
    Token closeBracket = tokenizer.getToken();
    if(!closeBracket.isCloseBrack())
        die("Parser::arrayInit", "Expected close brack, instead got", closeBracket);
    return arrayContents;
}
// are we parsing ID or Expr?
std::vector<std::string> Parser::parameter_list() {
    Token ID = tokenizer.getToken();
    std::string id = ID.getName();
    Token tok = tokenizer.getToken();
    std::vector<std::string> pList;
    while(tok.isComma()){
        pList.push_back(id);
        ID = tokenizer.getToken();
        id = ID.getName();
        tok = tokenizer.getToken();
    }
    pList.push_back(id);
    tokenizer.ungetToken();
    return pList;
}

// // array_ops: ID ’.’ ( ’append’ | ’pop’ ) ’(’ test ’)’
// ArrayOps *Parser::arrayOps(){
//     int opertation;
//
//     // Array 'ID'
//     Token arrayID = tokenizer.getToken();
//     if(!arrayID.isName())
//         die("Parser::ArrayOps", "Expected 'ArrayID', instead got", arrayID);
//
//     // .
//     Token arraydot = tokenizer.getToken();
//     if(!arraydot.isPeriod())
//         die("Parser::ArrayOps", "Expected an '.', instead got", arraydot);
//
//     // append/pop
//     Token arrayOp = tokenizer.getToken();
//     if(!arrayOp.isAppendOp() || !arrayOp.isPopOp() )
//         die("Parser::ArrayOps", "Expected an 'append' or 'pop', instead got", arrayOp);
//
//     if(arrayOp.isAppendKey())
//         operation = 1;
//
//     else if(arrayOp.isPopKey())
//         operation = 2;
//
//     // (
//     Token openParen = tokenizer.getToken();
//     if(!openParen.isOpenParen())
//         die("Parser::ArrayOps", "Expected an '(', instead got", openParen);
//
//     ExprNode *index = test(); // get index to append or pop
//
//     if(operation == 1 ){ // Append
//         // Perform operation on Array.index
//     }
//     else if( operation == 2) { // Pop
//         // Perform operation on Array.index
//     }
//
//     // )
//     Token closedParen = tokenizer.getToken();
//     if(!closedParen.isCloseParen())
//         die("Parser::ArrayOps", "Expected an '(', instead got", openParen);
//
//     return // FINISHED ARRAY
// }
//
// // array_len: ’len’ ’(’ ID ’)’
// ArrayLen *Parser::arrayLen(){
//     // len
//     Token arrayLen = tokenizer.getToken();
//     if(!arrayLen.isLenKey())
//         die("Parser::ArrayLen", "Expected an 'len', instead got", arrayLen);
//
//     // (
//     Token openParen = tokenizer.getToken();
//     if(!openParen.isOpenParen())
//         die("Parser::ArrayLen", "Expected an '(', instead got", openParen);
//
//     // Array 'ID'
//     Token arrayID = tokenizer.getToken();
//     if(!arrayID.isName())
//         die("Parser::ArrayLen", "Expected 'ArrayID', instead got", arrayID);
//
//     // )
//     Token closedParen = tokenizer.getToken();
//     if(!closedParen.isCloseParen())
//         die("Parser::ArrayOps", "Expected an '(', instead got", openParen);
//
//     return //Length of array
// }


std::vector<ExprNode*> Parser::testList() {
    ExprNode *left = test();
    Token tok = tokenizer.getToken();
    std::vector<ExprNode*> testlist;
    while(tok.isComma()){
        testlist.push_back(left);
        left = test();
        tok = tokenizer.getToken();
    }
    testlist.push_back(left);
    tokenizer.ungetToken();
    return testlist;
}

ExprNode *Parser::test() {
    // This function passes test onto or_test
    ExprNode *left = or_test();
    return left;
}

ExprNode *Parser::or_test() {
    // This function parses the grammar rules:
    // or_test: and_test {'or' and_test}*

    ExprNode *left = and_test();
    Token tok = tokenizer.getToken();
    //tok.getName() == "or"
    while (tok.isOr()) {
        InfixExprNode *p = new InfixExprNode(tok);
        p->left() = left;
        p->right() = and_test();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::and_test() {
    // This function parses the grammar rules:
    // and_test: not_test {'and' not_test}*

    ExprNode *left = not_test();
    Token tok = tokenizer.getToken();
    //tok.getName() == "and"
    while (tok.isAnd()) {
        InfixExprNode *p = new InfixExprNode(tok);
        p->left() = left;
        p->right() = not_test();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}
/*
ExprNode *Parser::not_test() {
    ExprNode *left = rel_expr()
    Token tok = tokenizer.getToken();

    if (tok.isNot()) {
        InfixExprNode *p = new InfixExprNode(tok);
        p->left() = left;
        p->right() = rel_expr();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}
*/
// imnplement not, just like factor for negatives
ExprNode *Parser::not_test() {
    // This function parses the grammar rules:
    // not_test: 'not' not_test | comparison
    // comparison = rel_expr() ?
    ExprNode *left = rel_expr();
    Token tok = tokenizer.getToken();

    // might not need isRelOp, needed for later grammer rule
    //tok.getName() == "not"
    while (tok.isNot()) {
        InfixExprNode *p = new InfixExprNode(tok);
        p->left() = left;
        p->right() = rel_expr();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::rel_expr(){
    //This function parses the grammar rules:

    // <rel_expr> -> <rel_term> {<rel_op> <rel_term>}
    // <rel_op> -> == | !=

    // However, it makes the <rel_op> left associative

    ExprNode *left = rel_primary();
    Token tok = tokenizer.getToken();
    while (tok.isEqualOperator() || tok.isGreaterThanOperator() ||
           tok.isLessThanOperator() || tok.isGreaterThanOrEqualOperator() ||
           tok.isLessThanOrEqualOperator() || tok.isNotEqualOperator()){
        InfixExprNode *p = new InfixExprNode(tok);
        p->left() = left;
        p->right() = rel_primary();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::rel_primary(){
    // This function parses the grammar rules:

    // <rel_primary> -> <arith_expr>

    ExprNode *left = expr();
    return left;
}

ExprNode *Parser::expr() {
    // This function parses the grammar rules:

    // <expr> -> <term> { <add_op> <term> }
    // <add_op> -> + | -

    // However, it makes the <add_op> left associative.

    ExprNode *left = term();
    Token tok = tokenizer.getToken();
    while (tok.isAdditionOperator() || tok.isSubtractionOperator()) {
        InfixExprNode *p = new InfixExprNode(tok);
        p->left() = left;
        p->right() = term();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}


ExprNode *Parser::term() {
    // This function parses the grammar rules:

    // <term> -> <primary> { <mult_op> <primary> }
    // <mult_op> -> * | / | %

    // However, the implementation makes the <mult-op> left associate.
    //ExprNode *left = primary();
    ExprNode *left = factor();
    Token tok = tokenizer.getToken();

    while (tok.isMultiplicationOperator() || tok.isDivisionOperator() || tok.isModuloOperator()) {
        InfixExprNode *p = new InfixExprNode(tok);
        p->left() = left;
        p->right() = factor();
        //p->right() = primary();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::factor() {
    // This function parses the grammar rules:

    // factor -> {'-'} factor | atom

    //ExprNode *left = primary();
    Token tok = tokenizer.getToken();
    if (tok.isSubtractionOperator()) {
        InfixExprNode *p = new InfixExprNode(tok);
        // Sets up token with the value of zero
        Token zero;
        zero.setWholeNumber(0);
        // Sets left side of infix expr to the
        // zero token
        p->left() = new WholeNumber(zero);
        p->right() = factor();
        ExprNode *left = p;
        return left;
    }

    tokenizer.ungetToken();
    ExprNode *left = primary();
    return left;

}

ExprNode *Parser::primary() {
    // This function parses the grammar rules:

    // <primary> -> [0-9]+
    // <primary> -> [_a-zA-Z]+
    // <primary> -> (<expr>)

    Token tok = tokenizer.getToken();
    //tok.print();
    if (tok.isWholeNumber() )
        return new WholeNumber(tok);
    else if( tok.isName() )
        return new Variable(tok);
    else if( tok.isStringValue() )
        return new String(tok); // added
    else if (tok.isOpenParen()) {
        ExprNode *p = test();
        Token token = tokenizer.getToken();
        if (!token.isCloseParen())
            die("Parser::primary", "Expected close-parenthesis, instead got", token);
        return p;
    }
    die("Parser::primary", "Unexpected token", tok);

    return nullptr;  // Will not reach this statement!
}
