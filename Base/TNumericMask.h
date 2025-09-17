#ifndef TNUMERICMASK_H
#define TNUMERICMASK_H
#include<map>

#include "arithmetic.h"


//todo: список переменных, а не только "x"

class TNumericMask
//примеры шаблонов
//  a*x^k
//  a*fun1(x) - пока нет, но хотелось бы
{
    int DebugNTabs;
    bool CheckTemplateConstant(const TNumeric& Template, const TNumeric& N, std::map<std::string, TNumeric> &Coefs);

    bool CheckTemplateCommunative(const TNumeric& Template, const TNumeric& N, std::map<std::string, TNumeric> &Coefs);

    TNumeric ToProd(const TNumeric& NConst);
    TNumeric ToSum(const TNumeric& NConst);
    TNumeric ToPow(const TNumeric& N);
    bool CheckTemplateProd(const TNumeric& Template, const TNumeric &NConst, std::map<std::string, TNumeric> &Coefs);
    bool CheckTemplatePow(const TNumeric& Template, const TNumeric& NConst, std::map<std::string, TNumeric> &Coefs);
    bool CheckTemplateFrac(const TNumeric& Template, const TNumeric& NConst, std::map<std::string, TNumeric> &Coefs);
    bool CheckTemplateSum(const TNumeric& Template, const TNumeric& NConst, std::map<std::string, TNumeric> &Coefs);
    bool CheckTemplateMinus(const TNumeric& Template, const TNumeric& NConst, std::map<std::string, TNumeric> &Coefs);
    bool AcceptCoefs(std::map<std::string, TNumeric> &Coefs, std::map<std::string, TNumeric> &NewCoefs); //добавляет NewCoefs к списку Coefs, если между ними нет противоречий
public:
    std::map<std::string, TNumeric> Coefs;

    std::vector<std::string> FindCoefsNames(const TNumeric& Template);
    bool CheckAllCoefsFound(const TNumeric& Template);

    TNumericMask();
    bool Match(const TNumeric& Template, const TNumeric& N, std::map<std::string, TNumeric> &Coefs);
    bool Match(const TNumeric& Template, const TNumeric& N);

    TNumeric Expand(const TNumeric& N);

};

#endif // TNUMERICMASK_H
