//
// Created by Ali A. Kooshesh on 2/5/19.
//

#include<iostream>
#include "ArithExpr.hpp"

// ExprNode
ExprNode::ExprNode(Token token): _token{token} {}

Token ExprNode::token() { return _token; }



// InfixExprNode functions
InfixExprNode::InfixExprNode(Token tk) : ExprNode{tk}, _left(nullptr), _right(nullptr) {}

ExprNode *&InfixExprNode::left() { return _left; }

ExprNode *&InfixExprNode::right() { return _right; }

// checks type of the right node of expr. Returns true if its 
// and int, false if it is a string
bool InfixExprNode::checkType(SymTab &symTab) {
    InfixExprNode *rNode = dynamic_cast<InfixExprNode*>(right());
    InfixExprNode *lNode = dynamic_cast<InfixExprNode*>(left());
    
    ExprNode *node = nullptr;
    if (rNode == nullptr) {
        node = dynamic_cast<ExprNode*>(right());
        if (node->token().isName()) {
        TypeDescriptor *desc = symTab.getValueFor(node->token().getName());
        if (desc->checkIfStr())
            return true;
        else 
            return false;
        }
        else if (node->token().isStringValue()) {
            return true;
        }
        else 
            return false;
    }
    else if (lNode == nullptr) {
        node = dynamic_cast<ExprNode*>(left());
        if (node->token().isName()) {
        TypeDescriptor *desc = symTab.getValueFor(node->token().getName());
        if (desc->checkIfStr())
            return true;
        else 
            return false;
        }
        else if (node->token().isStringValue()) {
            return true;
        }
        else 
            return false;
    }
    else
        return false;
}


int InfixExprNode::evaluate(SymTab &symTab) {
    // Evaluates an infix expression using a post-order traversal of the expression tree.
        
    if (!checkType(symTab)) {
        int lValue = left()->evaluate(symTab);
        int rValue = right()->evaluate(symTab);

        std::cout << "InfixExprNode::evaluate: " << lValue << " " << token().symbol() << " " << rValue << std::endl;
        if( token().isAdditionOperator() )
            return lValue + rValue;
        else if(token().isSubtractionOperator())
            return lValue - rValue;
        else if(token().isMultiplicationOperator())
            return lValue * rValue;
        else if(token().isDivisionOperator())
            return lValue / rValue; // division by zero?
        else if( token().isModuloOperator() )
            return lValue % rValue;
        else if ( token().isEqualOperator() )
            return lValue == rValue;
        else if ( token().isNotEqualOperator() )
            return lValue != rValue;
        else if ( token().isLessThanOperator() )
            return lValue < rValue;
        else if ( token().isGreaterThanOperator() )
            return lValue > rValue;
        else if ( token().isLessThanOrEqualOperator() )
            return lValue <= rValue;
        else if ( token().isGreaterThanOrEqualOperator() )
            return lValue >= rValue;
        else if (token().isOr())
            return lValue || rValue;
        else if (token().isAnd())
            return lValue && rValue;
        else if (token().isNot())
            return 0;
        else {
            std::cout << "InfixExprNode::evaluate: don't know how to evaluate this operator\n";
            token().print();
            std::cout << std::endl;
            exit(2);
        }
    }
    else {
        std::string lValue = left()->evaluateStr(symTab);
        std::string rValue = right()->evaluateStr(symTab);
        
        if ( token().isEqualOperator() )
            return lValue == rValue;
        else if ( token().isNotEqualOperator() )
            return lValue != rValue;
        else if ( token().isLessThanOperator() )
            return lValue < rValue;
        else if ( token().isGreaterThanOperator() )
            return lValue > rValue;
        else if ( token().isLessThanOrEqualOperator() )
            return lValue <= rValue;
        else if ( token().isGreaterThanOrEqualOperator() )
            return lValue >= rValue;
        else {
            std::cout << "InfixExprNode::evaluateStr RelOp: don't know how to evaluate this operator\n";
            token().print();
            std::cout << std::endl;
            exit(2);
        }
    }
    
}

void InfixExprNode::print() {
    _left->print();
    token().print();
    _right->print();
}

std::string InfixExprNode::evaluateStr(SymTab &symTab) {
    std::string lValue = left()->evaluateStr(symTab);
    std::string rValue = right()->evaluateStr(symTab);
    std::cout << "InfixExprNode::evaluateStr: " << lValue << " " << token().symbol() << " " << rValue << std::endl;
    
    if(token().isAdditionOperator())
        return lValue + rValue;
    else {
        std::cout << "InfixExprNode::evaluateStr: invalid operator for string evaluation\n";
        token().print();
        std::cout << std::endl;
        exit(2);
    }
        
}

// Range functions
forRange::forRange(Token token) : ExprNode{token}, _start{0}, _stop{0}, _step{1}, _itr{nullptr} {}

void forRange::setRange(std::vector<ExprNode*> itr, SymTab &symTab) {
    if (itr.size() == 3) {
        getStart() = itr[0]->evaluate(symTab);
        getStop() = itr[1]->evaluate(symTab);
        getStep() = itr[2]->evaluate(symTab);
    }
    else if (itr.size() == 2) {
        getStart() = itr[0]->evaluate(symTab);
        getStop() = itr[1]->evaluate(symTab);
        getStep() = 1;
    }
    else if (itr.size() == 1){
        getStop() = itr[0]->evaluate(symTab);
        getStart() = 0;
        getStep() = 1;
    }
}
int &forRange::getStart() {
    return _start;
}
int &forRange::getStop() {
    return _stop;
}
int &forRange::getStep() {
    return _step;
}
// Might need to use the declared var instead of getStart
// getStart might just be used to assign the for variable
bool forRange::rangeCont() {
    if (getStart() < 0 || getStop() < 0) {
        if (getStart() > getStop())
            return false;
        else
            return true;
    }
    else if (getStart() < getStop() )
        return false;
    else if (getStart() > getStop())
        return true;
    else
        return true;
}

int forRange::getNext() {
    getStart() += getStep();
    return getStart();
}

void forRange::print() {
    std::cout << "forRange" << std::endl;
}
int forRange::evaluate(SymTab &symTab) {
    return 0;
}
std::string forRange::evaluateStr(SymTab &symTab) {
    return "0";
}

// WHoleNumber
WholeNumber::WholeNumber(Token token): ExprNode{token} {}

void WholeNumber::print() {
    token().print();
}

int WholeNumber::evaluate(SymTab &symTab) {
    std::cout << "WholeNumber::evaluate: returning " << token().getWholeNumber() << std::endl;
    return token().getWholeNumber();
}

std::string WholeNumber::evaluateStr(SymTab &symTab) {
    return "0";
}

// String
String::String(Token token): ExprNode{token} {}

void String::print() {
    token().print();
}

int String::evaluate(SymTab &symTab) {
    return 0;
}

std::string String::evaluateStr(SymTab &symTab) {
    std::cout << "String::evaluate: returning " << token().getStringValue() << std::endl;
    return token().getStringValue();
}

// Variable

Variable::Variable(Token token): ExprNode{token} {}

void Variable::print() {
    token().print();
}

int Variable::evaluate(SymTab &symTab) {
    if( ! symTab.isDefined(token().getName())) {
        std::cout << "Use of undefined variable, " << token().getName() << std::endl;
        exit(1);
    }
    NumberDescriptor *desc = dynamic_cast<NumberDescriptor *>(symTab.getValueFor(token().getName()));
    std::cout << "Variable::evaluate: returning " << desc->value.intValue << std::endl;
    return desc->value.intValue;
}

std::string Variable::evaluateStr(SymTab &symTab) {
    if ( ! symTab.isDefined(token().getName())){
        std::cout << "Use of undefined variable, " << token().getName() << std::endl;
        exit(1);
    }
    StringDescriptor *desc = dynamic_cast<StringDescriptor *>(symTab.getValueFor(token().getName()));
    std::cout << "Variable::evaluateStr: returning " << desc->value.stringValue << std::endl;
    return desc->value.stringValue;
}

