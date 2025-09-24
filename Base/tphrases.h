#ifndef TPHRASES_H
#define TPHRASES_H

#include <fstream>
#include <map>
#include <string>
#include <vector>

#define LangRu 1
#define LangEn 2

class TMyTranslator {
    // Этой командной можно напечатать все строки-константы из программы
    // sed -n 's/\(".*"\)/\1/g;T;p' *.cpp
    // sed -n 's/.*\(".*"\).*/\1/g;T;p' *.cpp
  public:
    std::vector<std::string> Dictionaries;  // имена уже добавленных словарей
    int Language;
    bool Status;  // true, если tr завершилась удачно
    std::vector<std::string> Eng;
    std::vector<std::string> Rus;

    TMyTranslator();

    void AddEng(std::string str);
    void AddRus(std::string str);
    void AddDictionary(std::string str);
    bool CheckDictionary(std::string str);

    std::string tr(std::string Eng);
    void Assign(const TMyTranslator& Tr);
};

extern TMyTranslator MyTranslator;

#endif  // TPHRASES_H
