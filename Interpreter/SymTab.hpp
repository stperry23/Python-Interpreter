//
// Created by Ali A. Kooshesh on 2/5/19.
//

#ifndef EXPRINTER_SYMTAB_HPP
#define EXPRINTER_SYMTAB_HPP

#include <string>
#include <vector>
#include <map>
#include "TypeDesc.hpp"
//#include "Statements.hpp"

// This is a flat and integer-based symbol table. It allows for variables to be
// initialized, determines if a give variable has been defined or not, and if
// a variable has been defined, it returns its value.

class SymTab {
public:
    void setValueFor(std::string vName, int valueD);
    void setValueForString(std::string vName, std::string valueD);
    void setValueForArr(std::string vName, std::vector<int> elems);
    bool isDefined(std::string vName);
    TypeDescriptor *getValueFor(std::string vName);

private:
    std::map<std::string, TypeDescriptor *> symTab;
};

#endif //EXPRINTER_SYMTAB_HPP
