#ifndef THTMLWRITER_H
#define THTMLWRITER_H
#include <QThread>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "tline.h"
#include "tphrases.h"

#define __MULTITHREAD__

class THTMLWriter : public TRectangleElementVisitor {
    std::ofstream fout;
    std::string GetUniqueFileName(const char* Path, const char* ext);
    int UniqueFileCounter;
    bool BuildCSS(string filename);
    string Translate(string Str);

  public:
    vector<TMyTranslator*> Translators;
    vector<string> created_files;  // абсолютные пути до создаваемых файлов
    int NestingLevel;
    QFont FormulaFont, TextFont;
    QColor FormulaColor, TextColor, EditableColor;

    std::string Path;      // абсолютный путь к рабочей директории
    std::string filename;  // имя файла внутри рабочей директории
    THTMLWriter();
    ~THTMLWriter();

    virtual void visit(const TConstFormulaPlotter& element);
    virtual void visit(const THSpace& element);
    virtual void visit(const TLine& element);
    virtual void visit(const TLines& element);
    virtual void visit(const TText& element);

    bool BeginWrite(const char* filename);
    void WriteRectangleElement(const TRectangleElement* R);
    void EndWrite();

    void BeginSubSection();
    void EndSubSection();

    void BeginDiv(string DivName = "");
    void EndDiv();

    void BeginError();
    void EndError();

    void BeginParagraph();
    void EndParagraph();

    void NewLine();

    void IncrementNestingLevel();
    void DecrementNestingLevel();

    void WriteError(string Message);

    void AddHeader1(std::string str);
    void AddParagraph(std::string str);
    void AddParagraph(std::string str, int N);
    void AddParagraph(std::string str, const vector<const void*>& P);
    void AddParagraph(std::string str,
                      void* R);  // %N для формулы на отдельной строке, %n - для inline-формулы; также можно использовать %d
    void AddParagraph(std::string str, void* R1, void* R2);
    void AddParagraph(std::string str, void* R1, void* R2, void* R3);
    void AddParagraph(std::string str, void* R1, void* R2, void* R3, void* R4);
    void AddParagraph(
        std::string str,
        const TNumeric& N);  // %N для формулы на отдельной строке, %n - для inline-формулы; также можно использовать %d
    void AddParagraph(std::string str, const TNumeric& N1, const TNumeric& N2);
    void AddParagraph(std::string str, const TNumeric& N1, const int& D);
    void AddParagraph(std::string str, const TNumeric& N1, const TNumeric& N2, const TNumeric& N3);
    void AddParagraph(std::string str, const TNumeric& N1, const TNumeric& N2, const TNumeric& N3, const TNumeric& N4);

    void Add(std::string str);
    void AddFormula(const TConstFormulaPlotter& fp);       // на отдельной строке <br/> $$FORMULA$$ <br/>
    void AddInlineFormula(const TConstFormulaPlotter& N);  // встроенная в строку формула
    void AddFormula(const TNumeric& N);                    // на отдельной строке <br/> $$FORMULA$$ <br/>
    void AddInlineFormula(const TNumeric& N);              // встроенная в строку формула

    void PushTranslator(TMyTranslator* T);
    void PopTranslator();
    std::string GetFullPath();
    std::string GetFolder();
    std::string GetFileName();
};

#endif  // THTMLWRITER_H
