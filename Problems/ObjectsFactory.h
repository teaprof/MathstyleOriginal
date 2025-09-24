#ifndef OBJECTSFACTORY_H
#define OBJECTSFACTORY_H

#include "TPolynomProblems.h"

#include "tproblem.h"
/*#include "trationalsumproblem.h"
#include "TPolynomIntegralproblem.h"*/

/*#include "tderivativeproblem.h"
#include "tmaclaurinseriesproblem.h"
#include "tpowerproblems.h"
#include "tsystemofequations.h"
#include "ttrigonometryproblems.h"*/

std::shared_ptr<TProblem> LoadFromFile(ifstream& f, char* ResultMsg, int ResultMsgLen);
void SaveToFile(ofstream& f, std::shared_ptr<TProblem> P);
std::shared_ptr<TProblem> MakeCopy(std::shared_ptr<TProblem> P);

#endif  // OBJECTSFACTORY_H
