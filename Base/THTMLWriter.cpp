#include "THTMLWriter.h"

#include <QDir>
#include <QFileInfo>
#include <QFont>
#include <QImage>
#include <QPicture>
#include <QPixmap>

#include <iostream>
#include <sstream>
#include <variant>

#include "arithmetic.h"
#include "formulaplotter.h"

using namespace std;

THTMLWriter::THTMLWriter() {
    UniqueFileCounter = 0;
    FormulaFont = QFont("Serif", 12);
    FormulaColor = Qt::black;
    TextFont = QFont("Serif", 12);
    TextColor = Qt::black;
    EditableColor = Qt::black;

    NestingLevel = 0;
}

THTMLWriter::~THTMLWriter() {}

bool THTMLWriter::BuildCSS(string filename) {
    ofstream f(filename.c_str(), ios::out | ios::trunc);
    if (!f)
        return false;
    QString FontName = TextFont.family();
    QString Size;
    if (TextFont.pixelSize() >= 0)
        Size = QString::number(TextFont.pixelSize()) + "px";
    if (TextFont.pointSize() >= 0)
        Size = QString::number(TextFont.pointSize()) + "pt";
    QString InterlineSpacing;
    if (TextFont.pixelSize() >= 0)
        InterlineSpacing = QString::number(2.2 * TextFont.pixelSize()) + "px";
    if (TextFont.pointSize() >= 0)
        InterlineSpacing = QString::number(2.2 * TextFont.pointSize()) + "pt";
    QString Font = QString("font: normal ") + Size + "/" + InterlineSpacing + " " + FontName;
    string font = Font.toLocal8Bit().data();
    QString Color = QString("color: ") + TextColor.name();
    string color = Color.toLocal8Bit().data();

    f << "body { margin: 0; padding: 5px; " << font << "; " << color << "; text-align: left; background: #CCC; }" << endl;
    f << ".error { margin: 0; padding: 0; color: #FF0000; background: #FFF; }" << endl;
    f << ".nested { margin: 0; padding-left: 100px; border-left: 1px solid;}" << endl;
    f << ".cond { background: #EEF; padding: 20px; border: 1px solid; border-radius: 5px;}" << endl;
    f << ".sol { background: #DDE; padding: 20px; margin-top: 20px; border: 1px solid; border-radius: 5px;}" << endl;
    fout << ".sol h1 { color: #008; font-size: 1.5em; }" << endl;
    f.close();
    return true;
};

bool THTMLWriter::BeginWrite(const char* filename) {
    QFileInfo F(filename);

    Path = F.absoluteDir().path().toLocal8Bit().data();
    this->filename = F.fileName().toLocal8Bit().data();
    QDir Dir(F.absoluteDir());
    if (Dir.exists() == false)
        Dir.mkpath(Dir.absolutePath());
    QString CSSFileName = Dir.absolutePath() + "/style.css";
    if (BuildCSS(CSSFileName.toLocal8Bit().data()) == false)
        return false;

    fout.clear();
    fout.open(filename, ios::out | ios::trunc);
    if (!fout) {
        return false;
    };
    fout << "<html>" << endl;
    fout << "<head>" << endl;
    fout << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />" << endl;
    fout << "<link href=\"style.css\" rel=\"stylesheet\" type=\"text/css\" media=\"screen\" />" << endl;
    fout << "</head>" << endl;
    fout << "<body>" << endl;
    created_files.clear();
    created_files.push_back(string(F.absoluteFilePath().toLocal8Bit().data()));
    created_files.push_back(string(CSSFileName.toLocal8Bit().data()));
    return true;
}

void THTMLWriter::EndWrite() {
    fout << "</body></html>" << endl;
    fout.close();
}

void THTMLWriter::visit(const TConstFormulaPlotter& element) {
    AddInlineFormula(element);
    fout << endl;
}

void THTMLWriter::visit(const THSpace& element) {
    Q_UNUSED(element);
    // nothing to do
}

void THTMLWriter::visit(const TLine& Line) {
    fout << "<p>";
    for (size_t i = 0; i < Line.size(); i++)
        WriteRectangleElement(Line[i]);
    fout << "</p>" << endl;
}

void THTMLWriter::visit(const TLines& Lines) {
    fout << "<div style=\"padding: 20px; background-color: #FFCCCC;\">";
    for (size_t i = 0; i < Lines.size(); i++)
        WriteRectangleElement(Lines.at(i));
    fout << "</div>";
}

void THTMLWriter::visit(const TText& element) {
    AddParagraph(element.Text);
}

void THTMLWriter::WriteRectangleElement(const TRectangleElement* R) {
    R->accept(*this);
}

void THTMLWriter::WriteError(string Message) {
    fout << "<div class=\"error\">" << Translate(Message) << "</div>";
}
void THTMLWriter::BeginError() {
    BeginDiv("error");
}

void THTMLWriter::EndError() {
    EndDiv();
}

void THTMLWriter::AddHeader1(std::string str) {
    fout << "<h1>" << Translate(str) << "</h1>" << endl;
}

void THTMLWriter::AddParagraph(string str) {
    fout << "<p>" << Translate(str) << "</p>" << endl;
}
void THTMLWriter::AddParagraph(std::string format, int N) {
    vector<printable> v;
    v.push_back(N);
    AddParagraph(format, v);
}

void THTMLWriter::AddParagraph(std::string format, const vector<printable>& P) {
    format = Translate(format);
    size_t startindex = 0;
    size_t len = format.length();
    size_t count = 0;
    BeginParagraph();
    for (size_t curindex = 0; curindex < len; curindex++) {
        if (format[curindex] == '\\')
            curindex++;
        if (format[curindex] == '%') {
            string SubStr = format.substr(startindex, curindex - startindex);
            fout << SubStr << endl;
            curindex++;
            if (curindex >= len)
                break;
            char code = format[curindex];
            if (P.size() <= count) {
                cerr << "THTMLWriter::AddParagraph(string, vector): P.size() <= count, check format string, str = " << format << endl;
                throw "THTMLWriter::AddParagraph(string, vector): P.size() <= count, check format string str";
            };
            switch (code) {
                case 'd':
                case 'D': {
                    assert(std::holds_alternative<int>(P[count]));
                    int i = std::get<int>(P[count]);
                    fout << i;
                    break;
                };
                case 'n': {
                    assert(std::holds_alternative<std::shared_ptr<TNumeric>>(P[count]));
                    auto R = std::get<std::shared_ptr<TNumeric>>(P[count]);
                    TConstFormulaPlotter plotter(R);
                    WriteRectangleElement(&plotter);
                    break;
                }
                case 'N': {
                    assert(std::holds_alternative<std::shared_ptr<TNumeric>>(P[count]));
                    auto R = std::get<std::shared_ptr<TNumeric>>(P[count]);
                    TConstFormulaPlotter plotter(R);
                    NewLine();
                    WriteRectangleElement(&plotter);
                    NewLine();
                    break;
                }
                default:
                    cerr << "THTMLWriter::AddParagraph(string, vector): unknown format letter, check format string, str = " << format << endl;
                    throw "THTMLWriter::AddParagraph(string, vector): unknown format letter, check format string str";
                    break;
            }
            count++;
            startindex = curindex + 1;
        }
    }
    string SubStr = format.substr(startindex);
    fout << SubStr << endl;
    EndParagraph();
}

void THTMLWriter::AddParagraph2(std::string format, void* R) {
    vector<printable> V;
    V.push_back(R);
    AddParagraph(format, V);
}

void THTMLWriter::AddParagraph(std::string format, void* R1, void* R2) {
    vector<printable> V;
    V.push_back(R1);
    V.push_back(R2);
    AddParagraph(format, V);
}

void THTMLWriter::AddParagraph(std::string format, void* R1, void* R2, void* R3) {
    vector<printable> V;
    V.push_back(R1);
    V.push_back(R2);
    V.push_back(R3);
    AddParagraph(format, V);
}
void THTMLWriter::AddParagraph(std::string format, void* R1, void* R2, void* R3, void* R4) {
    vector<printable> V;
    V.push_back(R1);
    V.push_back(R2);
    V.push_back(R3);
    V.push_back(R4);
    AddParagraph(format, V);
}

void THTMLWriter::AddParagraph(std::string format, const TNumeric& N) {
    vector<printable> V;
    V.push_back(&N);
    AddParagraph(format, V);
}
void THTMLWriter::AddParagraph(std::string format, const TNumeric& N1, const TNumeric& N2) {
    vector<printable> V;
    V.push_back(&N1);
    V.push_back(&N2);
    AddParagraph(format, V);
}
void THTMLWriter::AddParagraph2(std::string format, const TNumeric& N1, const int& D) {
    vector<printable> V;
    V.push_back(&N1);
    V.push_back(&D);
    AddParagraph(format, V);
}

void THTMLWriter::AddParagraph(std::string format, const TNumeric& N1, const TNumeric& N2, const TNumeric& N3) {
    vector<printable> V;
    V.push_back(&N1);
    V.push_back(&N2);
    V.push_back(&N3);
    AddParagraph(format, V);
}
void THTMLWriter::AddParagraph(std::string format, const TNumeric& N1, const TNumeric& N2, const TNumeric& N3, const TNumeric& N4) {
    vector<printable> V;
    V.push_back(&N1);
    V.push_back(&N2);
    V.push_back(&N3);
    V.push_back(&N4);
    AddParagraph(format, V);
}

void THTMLWriter::Add(string str) {
    fout << Translate(str) << endl;
}

void THTMLWriter::BeginSubSection() {
    fout << "<div style=\"padding: 20px\">";
}

void THTMLWriter::EndSubSection() {
    fout << "</div>";
}

void THTMLWriter::BeginDiv(string DivName) {
    if (DivName.empty())
        fout << "<div>" << endl;
    else
        fout << "<div class=\"" << DivName << "\">" << endl;
}

void THTMLWriter::EndDiv() {
    fout << "</div>" << endl;
}

void THTMLWriter::BeginParagraph() {
    fout << "<p>";
}

void THTMLWriter::EndParagraph() {
    fout << "</p>" << endl;
}

void THTMLWriter::NewLine() {
    fout << "<br/>" << endl;
}

void THTMLWriter::IncrementNestingLevel() {
    NestingLevel++;
    BeginDiv("nested");
}

void THTMLWriter::DecrementNestingLevel() {
    NestingLevel--;
    EndDiv();
}

class TNumericExporter  //: public QThread
{
  public:
    TNumericExporter(const TConstFormulaPlotter& formula_plotter) : fp(formula_plotter) {}
    const TConstFormulaPlotter& fp;
    string FullFileName;
    QColor EditableColor, TextColor, FormulaColor;
    QFont FormulaFont, TextFont;
    void run();
};

void TNumericExporter::run() {
    QPicture Pic;
    QPainter Painter;
    Painter.begin(&Pic);
    auto C = std::make_shared<TPaintCanvas>(&Painter);
    C->EditableColor = EditableColor;
    C->TextColor = TextColor;
    C->FormulaColor = FormulaColor;
    C->FormulaFont = FormulaFont;
    C->TextFont = TextFont;
    auto [W, H, D] = fp.GetTextRectangle(C);
    fp.DrawAtBaseLeft(C, 0, 0);
    Painter.end();
    QRect Rect(0, -H, W, H + D);
    QSize Size = Rect.size();
    QImage Im(Size, QImage::Format_ARGB32_Premultiplied);  //(Size, QImage::Format_RGB888);
    Im.fill(qRgba(0, 0, 0, 0));
    QPainter Painter2;
    if (Painter2.begin(&Im)) {
        // Painter2.fillRect(0, 0, Rect.width(), Rect.height(), QColor(0x80, 0x80, 0x80, 0x80));
        // Painter2.drawRect(0, 0, Rect.width()-1, Rect.height()-1);
        Painter2.drawPicture(-Rect.topLeft(), Pic);
        Painter2.end();
    } else {
        cerr << "THTMLWriter::AddInlineFormula: the size of image is zero" << endl;
        // auto [W, H, D] = fp.GetTextRectangle(&C); //for debug purposes only
    }

    Im.save(QString::fromLocal8Bit(FullFileName.c_str()));
}

void THTMLWriter::AddInlineFormula(const TConstFormulaPlotter& fp) {
    string FileName = GetUniqueFileName(Path.c_str(), "png");
    QFileInfo FileInfo(QString::fromLocal8Bit(Path.c_str()), QString::fromLocal8Bit(FileName.c_str()));
    string AbsoluteFilePath = FileInfo.absoluteFilePath().toLocal8Bit().data();
    created_files.push_back(AbsoluteFilePath);
    TNumericExporter* W = new TNumericExporter(fp);
    W->FullFileName = AbsoluteFilePath;
    W->FormulaColor = FormulaColor;
    W->EditableColor = EditableColor;
    W->TextColor = TextColor;
    W->FormulaFont = FormulaFont;
    W->TextFont = TextFont;
    W->run();

    stringstream padstr;
    padstr << "style=\"vertical-align: middle;\"";
    // fout<<"<img src=\""<<FileName<<"\" align = \"bottom\" "<<padstr.str()<<" alt = \""<<N.CodeBasic()<<"\"/>";
    fout << "<img src=\"" << FileName << "\" align = \"bottom\" " << padstr.str() << " alt = \"" << TFormulaPlotter::CodeBasic(fp.numeric()) << "\"/>";
    /*fout<<"some text";
    fout<<"<img src=\""<<FileName<<"\" align = \"middle\" "<<padstr.str()<<" alt = \""<<N.CodeBasic()<<"\"/>";*/
}

void THTMLWriter::AddFormula(const TConstFormulaPlotter& fp)  /// \todo: rename to printFormula since it doesn't take the ownership of fp (here and others)
{
    fout << "<br/>" << endl;
    AddInlineFormula(fp);
    fout << "<br/>" << endl;
}

void THTMLWriter::AddFormula(const TNumeric& N) {
    AddFormula(TConstFormulaPlotter(std::make_shared<TNumeric>(N)));
}
void THTMLWriter::AddInlineFormula(const TNumeric& N) {
    AddInlineFormula(TConstFormulaPlotter(std::make_shared<TNumeric>(N)));
}

string THTMLWriter::GetUniqueFileName(const char* Path, const char* ext) {
    Q_UNUSED(Path);
    UniqueFileCounter++;
    stringstream str;
    str << UniqueFileCounter << "." << ext;
    return str.str();
}

string THTMLWriter::GetFullPath() {
    return Path + "/" + filename;
}
string THTMLWriter::GetFolder() {
    return Path;
}
string THTMLWriter::GetFileName() {
    return filename;
}

void THTMLWriter::PushTranslator(TMyTranslator* T) {
    Translators.push_back(T);
}

void THTMLWriter::PopTranslator() {
    if (Translators.size() > 0)
        Translators.pop_back();
}
string THTMLWriter::Translate(string Str) {
    if (Translators.size() == 0)
        return Str;
    size_t i = Translators.size();
    string res;
    do {
        i--;
        res = Translators[i]->tr(Str);
        if (Translators[i]->Status)
            return res;
    } while (i > 0);
    return Str;
}
