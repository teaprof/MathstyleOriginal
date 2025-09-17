#ifndef OBJECTSFACTORY_H
#define OBJECTSFACTORY_H

#include "tproblem.h"
#include "TPolynomProblems.h"
#include "trationalsumproblem.h"
#include "tpowerproblems.h"
#include "ttrigonometryproblems.h"
#include "tsystemofequations.h"
#include "tderivativeproblem.h"
#include "tmaclaurinseriesproblem.h"
#include "TPolynomIntegralproblem.h"

TProblem* LoadFromFile(ifstream &f, char* ResultMsg, int ResultMsgLen);
void SaveToFile(ofstream &f, TProblem *P);
TProblem* MakeCopy(TProblem* P);

#endif // OBJECTSFACTORY_H
