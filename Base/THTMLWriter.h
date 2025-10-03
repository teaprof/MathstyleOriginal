#ifndef THTMLWRITER_H
#define THTMLWRITER_H
#include <QThread>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <variant>

#include "tline.h"
#include "tphrases.h"
#include "formulaplotter.h"

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

    using printable = std::variant<int, std::shared_ptr<TNumeric>>;

    void AddHeader1(std::string str);
    
     // %N для формулы на отдельной строке, %n - для inline-формулы; также можно использовать %d
    template<class ... Args>
    void addParagraph(const std::string& format, Args ... args) {
      BeginParagraph();
      parseFormatString(format, printable(args)...);
      EndParagraph();
    }

    void parseFormatString(const std::string& format) {
      fout<<format;
    }

    template<class Arg, class ... Args>
    void parseFormatString(const std::string& format, Arg arg, Args ... args) {
      size_t pos = format.find('%');
      assert(pos != std::string::npos);
      assert(pos + 1 < format.size());
      fout << format.substr(0, pos);
      const char& format_char = format[pos+1];
      switch (format_char) {
          case 'd':
          case 'D': {
              assert(std::holds_alternative<int>(arg));
              int i = std::get<int>(arg);
              fout << i;
              break;
          };
          case 'n': {
              assert(std::holds_alternative<std::shared_ptr<TNumeric>>(arg));
              auto R = std::get<std::shared_ptr<TNumeric>>(arg);              
              TConstFormulaPlotter plotter(R);
              WriteRectangleElement(&plotter);
              break;
          }
          case 'N': {
              assert(std::holds_alternative<std::shared_ptr<TNumeric>>(arg));
              auto R = std::get<std::shared_ptr<TNumeric>>(arg);
              TConstFormulaPlotter plotter(R);
              NewLine();
              WriteRectangleElement(&plotter);
              NewLine();
              break;
          }
          default:
            assert(false); 
      }
      parseFormatString(format.substr(pos+2, format.length()), std::forward<Args>(args)...);
    }

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
