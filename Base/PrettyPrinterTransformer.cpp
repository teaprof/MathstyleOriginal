#include "PrettyPrinterTransformer.h"

#include <algorithm>
#include <cassert>

PNumeric PrettyPrinterTransformer::convert(const PNumeric numeric) {
    auto res = TNumeric::create(*numeric);
    res->operands.clear();
    for(const auto& child : numeric->operands) {
        res->operands.push_back(PrettyPrinterTransformer::convert(child));
    }
    switch(res->operation) {
        case OperatorSum: 
        res = PrettyPrinterTransformer::convertSum(res);
        break;
        default:
        break;
    }
    return res;
}

/*  rules for Sum:
        removeZeroTerms
        collapseSigns
        removeFirstPlus

    rules for Prod:
        removeOnes  1*x => x, -1*x => -1*x
        collapseSigns            
*/
PNumeric PrettyPrinterTransformer::convertSum(const PNumeric numeric) {    
    assert(numeric->operation == OperatorSum);

    // remove all terms represented by 0 or like a*0
    vector<size_t> exclude_indicies;  
    for (size_t i = 0; i < numeric->operands.size(); i++) {
        if (numeric->operands[i]->operation == OperatorConst && numeric->operands[i]->strval == "0") {
            exclude_indicies.push_back(i);
        }
        if (numeric->operands[i]->operation == OperatorFrac) {
            if (numeric->operands[i]->operands[0]->operation == OperatorConst && numeric->operands[i]->operands[0]->strval == "0") {
                exclude_indicies.push_back(i);
            }
        }
        if (numeric->operands[i]->operation == OperatorProd) {
            for (size_t j = 0; j < numeric->operands[i]->operands.size(); j++)
                if (numeric->operands[i]->operands[j]->operation == OperatorConst && numeric->operands[i]->operands[j]->strval == "0") {
                    exclude_indicies.push_back(i);
                    break;
                };
        }
    }

    auto res = TNumeric::create(*numeric);
    res->operands.clear();
    for(size_t idx = 0; idx < numeric->operands.size(); idx++) {
        if(!std::binary_search(exclude_indicies.begin(), exclude_indicies.end(), idx)) {
            res->operands.push_back(numeric->operands[idx]);
        }

    }

    // if all terms are zeros we should leave single 0 as the answer
    if (res->operands.size() == 0)
    {
        res->operands.push_back(TNumeric::create(0));
    };
    return res;

}
