//
// Created by Ali A. Kooshesh on 2/5/19.
//

#include <iostream>
#include "SymTab.hpp"


// parameter changed to auto from int
void SymTab::setValueFor(std::string vName, int valueD) {
    // Define a variable by setting its initial value.
    std::cout << vName << " <- " << valueD << std::endl;
    NumberDescriptor *desc = new NumberDescriptor(TypeDescriptor::INTEGER);
    desc->value.intValue = valueD;
    symTab[vName] = desc; 
}

void SymTab::setValueForString(std::string vName, std::string valueD){
    std::cout << vName << " <- " << valueD << std::endl;
    StringDescriptor *desc = new StringDescriptor(TypeDescriptor::STRING);
    desc->value.stringValue = valueD;
    symTab[vName] = desc;
}

void SymTab::setValueForArr(std::string vName, std::vector<int> elems) {
    std::cout << vName << " <- array " << std::endl;
    ArrayDescriptor *desc = new ArrayDescriptor(TypeDescriptor::ARRAY);
    desc->value.intArr = elems;
    symTab[vName] = desc;
}


bool SymTab::isDefined(std::string vName) {
    return symTab.find(vName) != symTab.end();
}

TypeDescriptor *SymTab::getValueFor(std::string vName) {
    if( ! isDefined(vName)) {
        std::cout << "SymTab::getValueFor: " << vName << " has not been defined.\n";
        exit(1);
    }
    NumberDescriptor *desc = dynamic_cast<NumberDescriptor *>(symTab.find(vName)->second);
    if (desc == nullptr){
        StringDescriptor *sDesc = dynamic_cast<StringDescriptor *>(symTab.find(vName)->second);
        std::cout << "SymTab::getValueForStr: " << vName << " contains " << sDesc->value.stringValue << std::endl;
        return sDesc;
    }
    else {
        std::cout << "SymTab::getValueFor: " << vName << " contains " << desc->value.intValue << std::endl;
        return desc;
    }
}

