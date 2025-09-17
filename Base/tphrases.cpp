#include<stdio.h>
#include<iostream>
#include<algorithm>
#include "tphrases.h"

using namespace std;
//***********************************************************
TMyTranslator::TMyTranslator()
{
    Language = LangEn;
}

void TMyTranslator::AddEng(string str)
{
    Eng.push_back(str);
    if(Rus.size() + 1!= Eng.size())
        throw "TMyTranslator::AddRus(string str): AddRus must be called once after every AddEng";
}

void TMyTranslator::AddRus(string str)
{
    Rus.push_back(str);
    if(Rus.size() != Eng.size())
        throw "TMyTranslator::AddRus(string str): AddRus must be called once after every AddEng";
}
void TMyTranslator::AddDictionary(std::string str)
{
    if(CheckDictionary(str) == false)
        Dictionaries.push_back(str);
}
bool TMyTranslator::CheckDictionary(std::string str)
{
    if(find(Dictionaries.begin(), Dictionaries.end(), str) != Dictionaries.end())
        return true;
    else
        return false;
}

string TMyTranslator::tr(string str)
{
    Status = true;
    if(Language == LangEn) return str;

    //else russian language
    for(size_t i = 0; i < Eng.size(); i++)
    {
        if(Eng[i] == str)
            return Rus[i];
    };
#ifdef __DEBUG__
    cout<<"TMyTranslator: unknown str: \""<<str<<"\""<<endl;
#endif
    Status = false;
    return str;
}
void TMyTranslator::Assign(const TMyTranslator& Tr)
{
    Eng = Tr.Eng;
    Rus = Tr.Rus;
}
TMyTranslator MyTranslator;//один единый глобальный словарь для всех
