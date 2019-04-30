//
// Created by Ali A. Kooshesh on 2/5/19.
//

#include "Statements.hpp"

// Statement
Statement::Statement() {}

// Statements

Statements::Statements() {}
void Statements::addStatement(Statement *statement) { _statements.push_back(statement); }

void Statements::print() {
    for (auto s: _statements)
        s->print();
}

void Statements::evaluate(SymTab &symTab) {
    for (auto s: _statements)
        s->evaluate(symTab);
}

// AssignmentStatement
//AssignmentStatement::AssignmentStatementArray() : _lhsVariable{""},
//_rhsExpressionArray{nullptr} {}

//AssignmentStatements::AssignmentStatementArray(std::string _lhsVar, std::vector<ExprNode*>):
 //       _lhsVariable{lhsVar}, _rhsExpression{rhsExprArray} {}

AssignmentStatement::AssignmentStatement() : _lhsVariable{""}, _rhsExpression{nullptr}, _arrayContent{nullptr} {}

AssignmentStatement::AssignmentStatement(std::string lhsVar, ExprNode *rhsExpr, std::vector<ExprNode*> arrCont):
        _lhsVariable{lhsVar}, _rhsExpression{rhsExpr}, _arrayContent{arrCont} {}

void AssignmentStatement::evaluate(SymTab &symTab) {
    // Handles variables
    //std::cout << "printitnitn" << std::endl;
    if (rhsExpression() == nullptr) {
        std::vector<ExprNode*> arr = arrayContent();
        // need to add checks for int or string array, and 
        // create the array based off that
        std::vector<int> temp;
        // SymTab set value for array
        for (int i = 0; i < arr.size(); i++) {
            int element = arr[i]->evaluate(symTab);
            temp.push_back(element);
        }
        symTab.setValueForArr(lhsVariable(), temp);
    }
    else {
        if (rhsExpression()->token().isName()) {
            TypeDescriptor *desc = symTab.getValueFor(rhsExpression()->token().getName());
            if (desc->checkIfInt()){
                int rhs = rhsExpression()->evaluate(symTab);
                symTab.setValueFor(lhsVariable(), rhs);
            }
            else {
                std::string rhs = rhsExpression()->evaluateStr(symTab);
                symTab.setValueForString(lhsVariable(), rhs);
            }
        } // Handles string values
        else if (rhsExpression()->token().isStringValue()) {
            std::string rhs = rhsExpression()->evaluateStr(symTab);
            symTab.setValueForString(lhsVariable(), rhs);
        } // Handles integer values
        else if (rhsExpression()->token().isWholeNumber()) {
            int rhs = rhsExpression()->evaluate(symTab);
            symTab.setValueFor(lhsVariable(), rhs);
        } // Handles infix expressions for strings and ints
        else if (rhsExpression()->token().isArithmeticOperator()){
            InfixExprNode *lNode = dynamic_cast<InfixExprNode*>(rhsExpression());
            lNode->right()->token().print();
            while(lNode->right()->token().isArithmeticOperator()) {
                lNode = dynamic_cast<InfixExprNode*>(lNode->right());
            }
            if (lNode->right()->token().isName()){
                TypeDescriptor *desc = symTab.getValueFor(lNode->right()->token().getName());
                    if (desc->checkIfInt()){
                        int rhs = rhsExpression()->evaluate(symTab);
                        symTab.setValueFor(lhsVariable(), rhs);
                    }
                    else {
                        std::string rhs = rhsExpression()->evaluateStr(symTab);
                        symTab.setValueForString(lhsVariable(), rhs);
                    }
            }
            else if (lNode->right()->token().isWholeNumber()) {
                int rhs = rhsExpression()->evaluate(symTab);
                symTab.setValueFor(lhsVariable(), rhs);
            }
            else {
                rhsExpression()->token().print();
                std::string rhs = rhsExpression()->evaluateStr(symTab);
                symTab.setValueForString(lhsVariable(), rhs);
            }

        }
        else if (rhsExpression()->token().isRelationalOperator()) {
            std::cout << std::endl;
            std::cout << std::endl;
            int rhs = rhsExpression()->evaluate(symTab);
            symTab.setValueFor(lhsVariable(), rhs);
        }
    }
}

std::string &AssignmentStatement::lhsVariable() {
    return _lhsVariable;
}

ExprNode *&AssignmentStatement::rhsExpression() {
    return _rhsExpression;
}
std::vector<ExprNode*> &AssignmentStatement::arrayContent() {
    return _arrayContent;
}

void AssignmentStatement::print() {
    std::cout << _lhsVariable << " = ";
    _rhsExpression->print();
    std::cout << std::endl;
}

PrintStatement::PrintStatement() : _printExpression{nullptr} {}

PrintStatement::PrintStatement(std::vector<ExprNode*> printExpr) : 
        _printExpression{printExpr} {} 

void PrintStatement::evaluate(SymTab &symTab) {
    std::cout << "Print function found" << std::endl;
    std::string consolePrint;
    std::vector<ExprNode*> l = printExpression();
    // Vector of all the ExprNodes in test list
    for (int i = 0; i < l.size(); i++) {
        
        // Handles printing variables
        if (l[i]->token().isName()) {
            TypeDescriptor *desc = symTab.getValueFor(l[i]->token().getName());
            if (desc->checkIfInt()) {
                int rhs = l[i]->evaluate(symTab);
                consolePrint += std::to_string(rhs);
            }
            else {
                std::string rhs = l[i]->evaluateStr(symTab);
                consolePrint += rhs;
            }
        }   // Handles string variable
        else if (l[i]->token().isStringValue()) {
            std::string rhs = l[i]->evaluateStr(symTab);
            consolePrint += rhs;
        }   // Handles interger variable
        else if (l[i]->token().isWholeNumber()) {
            int rhs = l[i]->evaluate(symTab);
            consolePrint += std::to_string(rhs);
        }
        // Handles infix expressions for strings integers and variables
        else if (l[i]->token().isArithmeticOperator()){
            
            InfixExprNode *lNode = dynamic_cast<InfixExprNode*>(l[i]);
            
            while(lNode->right()->token().isArithmeticOperator()) {
                lNode = dynamic_cast<InfixExprNode*>(lNode->right());
            }
            // Variable
            if (lNode->right()->token().isName()) {
                TypeDescriptor *desc = symTab.getValueFor(lNode->right()->token().getName());
                // Int Variable
                if (desc->checkIfInt()){
                    int rhs = l[i]->evaluate(symTab);
                    consolePrint += std::to_string(rhs);
                }
                // String Variable
                else {
                    std::string rhs = l[i]->evaluateStr(symTab);
                    consolePrint += rhs;
                }
            }
            // Integer
            else if (lNode->right()->token().isWholeNumber()) {
                int rhs = l[i]->evaluate(symTab);
                consolePrint += std::to_string(rhs);
            } // String
            else {
                std::string rhs = l[i]->evaluateStr(symTab);
                consolePrint += rhs;
            }
        }
        else if (l[i]->token().isRelationalOperator()) {
            int rhs = l[i]->evaluate(symTab);
            consolePrint += std::to_string(rhs);
        }
        consolePrint += " ";
    }
    std::cout << std::endl;
    std::cout << "Output from Terminal: " << std::endl;
    std::cout << std::setw(50) << consolePrint << std::endl;
}


std::vector<ExprNode*> &PrintStatement::printExpression() {
    return _printExpression;
}

void PrintStatement::print() {
    std::cout << "print function will print : " << std::endl;
    //_printVariable->print();
    //printExpression()->token().print();
    std::cout << std::endl;
}

// ForStatement

ForStatement::ForStatement() : _varName{""}, _r{nullptr}, _list{nullptr}, _statements{nullptr} {}

ForStatement::ForStatement(std::string var, forRange *r, std::vector<ExprNode*> list, Statements *stmts):
    _varName{var}, _r{r}, _list{list}, _statements{stmts} {}

void ForStatement::evaluate(SymTab &symTab) {
    forLoopR()->setRange(rangeList(), symTab);
    int varVal = forLoopR()->getStart();
    symTab.setValueFor(forLoopVar(), varVal);
    std::cout << "Stop: " << forLoopR()->getStop() << std::endl;
    while(!forLoopR()->rangeCont()){
        //std::cout << "Made it here" << std::endl;
        statements()->evaluate(symTab);
        varVal = forLoopR()->getNext();
        symTab.setValueFor(forLoopVar(), varVal);
    }
    std::cout << "forloop reset" << std::endl;
    forLoopR()->setRange(rangeList(), symTab);
    
}
std::string &ForStatement::forLoopVar() {
    return _varName;
}
std::vector<ExprNode*> &ForStatement::rangeList() {
    return _list;
}
Statements *&ForStatement::statements(){
    return _statements;
}
forRange *&ForStatement::forLoopR() {
    return _r;
}

void ForStatement::print() {
    std::cout << "Statements that have been initialized for this for loop are: " << std::endl;
    statements()->print();
    std::cout << std::endl;
}

// IfStatement

IfStatement::IfStatement() : _conditions{nullptr}, _statements{nullptr}, _elseStatements{nullptr}, 
    _elifStmtList{nullptr}, _elifCondList{nullptr} {}

IfStatement::IfStatement(ExprNode *cond, Statements *stmts, Statements *elseStmts, std::vector<Statements*> stmtList,
                        std::vector<ExprNode*> condList): 
    _conditions{cond}, _statements{stmts}, _elseStatements{elseStmts}, _elifStmtList{stmtList}, _elifCondList{condList} {}

void IfStatement::evaluate(SymTab &symTab) {
    std::vector<Statements*> elif_s = elifStmtList();
    std::vector<ExprNode*> elif_c = elifCondList();
    bool elifCaught = false;
    if (elif_s.empty() && elif_c.empty()) {
        if (elseStatements() == nullptr) {

            if (conditions()->evaluate(symTab)){
                statements()->evaluate(symTab);
            }
        }
        else {
            if (conditions()->evaluate(symTab)){
                statements()->evaluate(symTab);
            }
            else {
                elseStatements()->evaluate(symTab);
            }
        }
    }
    else {
        if (conditions()->evaluate(symTab)){
                statements()->evaluate(symTab);
        }
        else {
            for (int i = 0; i < elif_s.size(); i++) {
                if (elif_c[i]->evaluate(symTab)) {
                    elif_s[i]->evaluate(symTab);
                    elifCaught = true;
                    break;
                }
            }
            if (!elifCaught && elseStatements() != nullptr) {
                elseStatements()->evaluate(symTab);
            }
        }
    }
}


ExprNode *&IfStatement::conditions() {
    return _conditions;
}

Statements *&IfStatement::statements() {
    return _statements;
}

Statements *&IfStatement::elseStatements() {
    return _elseStatements;
}

std::vector<Statements*> &IfStatement::elifStmtList() {
    return _elifStmtList;
}

std::vector<ExprNode*> &IfStatement::elifCondList() {
    return _elifCondList;
}

void IfStatement::print() {
    std::cout << "If statements that will be evaluated: " << std::endl;
    statements()->print();
    std::cout << "if condition: " << std::endl;
    conditions()->print();
    std::cout << std::endl;
    std::cout << "Is true" << std::endl;
}

// Function List

FuncList::FuncList() : {}

void FuncList::evaluate(SymTab &funcTab) {
    std::cout << "This does nothing" << std::endl;
}

void FuncList::addFunc(std::string funcName, FuncStatement *func) {
    std::cout << "Adding " << funcName << "to the function list" << std::endl;
    funcTab[funcName] = func;
}

FuncStatement FuncList::findFunc(std::string funcName) {
    return funcTab.find(funcName)->second
}

void FuncList::print() {
    std::cout << "This is our function list" << std::endl;
}


// Function statement

FuncStatement::FuncStatement() : _functionName{""}, _params{nullptr}, _funcStmts{nullptr} {}

FuncStatement::FuncStatement(std::string funcName, std::vector<std::string> funcParams, Statements *funcStmts ):
    _functionName{funcName}, _params{funcParams}, _funcStmts{funcStmts} {}

void FuncStatment::evaluate(SymTab &symTab) {
    //evaluate the function statements
    std::cout << "need to add" << std::endl;
}

std::vector<std::string> &FuncStatement::funcParams() {
    return _params;
}

std::string &FuncStatement::functionName() {
    return _functionName;
}

Statements *&FuncStatement::funcStatements() {
    return _funcStmts;
}

void FuncStatements::print() {
    std::cout << "Function print" << std::endl;
}








