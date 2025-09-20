#include <QFileInfo>
#include <QDir>
#include <QPicture>
#include <QFont>
#include <QImage>
#include <QPixmap>
#include <sstream>
#include <iostream>

#include "arithmetic.h"
#include "formulaplotter.h"
#include "THTMLWriter.h"

using namespace std;

THTMLWriter::THTMLWriter()
{
    UniqueFileCounter = 0;
    FormulaFont = QFont("Serif", 12);
    FormulaColor = Qt::black;
    TextFont = QFont("Serif", 12);
    TextColor = Qt::black;
    EditableColor = Qt::black;

    NestingLevel = 0;
}

THTMLWriter::~THTMLWriter()
{

}

bool THTMLWriter::BuildCSS(string filename)
{
    ofstream f(filename.c_str(), ios::out | ios::trunc);
    if(!f)return false;
    QString FontName = TextFont.family();
    QString Size;
    if(TextFont.pixelSize() >= 0) Size = QString::number(TextFont.pixelSize())+"px";
    if(TextFont.pointSize() >= 0) Size = QString::number(TextFont.pointSize())+"pt";
    QString InterlineSpacing;
    if(TextFont.pixelSize() >= 0) InterlineSpacing = QString::number(2.2*TextFont.pixelSize())+"px";
    if(TextFont.pointSize() >= 0) InterlineSpacing = QString::number(2.2*TextFont.pointSize())+"pt";
    QString Font = QString("font: normal ")+Size+ "/"+InterlineSpacing+" "+FontName;
    string font = Font.toLocal8Bit().data();
    QString Color = QString("color: ") + TextColor.name();
    string color = Color.toLocal8Bit().data();

    f<<"body { margin: 0; padding: 5px; "<<font<<"; "<<color<<"; text-align: left; background: #CCC; }"<<endl;
    f<<".error { margin: 0; padding: 0; color: #FF0000; background: #FFF; }"<<endl;
    f<<".nested { margin: 0; padding-left: 100px; border-left: 1px solid;}"<<endl;
    f<<".cond { background: #EEF; padding: 20px; border: 1px solid; border-radius: 5px;}"<<endl;
    f<<".sol { background: #DDE; padding: 20px; margin-top: 20px; border: 1px solid; border-radius: 5px;}"<<endl;
    fout<<".sol h1 { color: #008; font-size: 1.5em; }"<<endl;
    f.close();
    return true;
};

bool THTMLWriter::BeginWrite(const char* filename)
{
    QFileInfo F(filename);

    Path = F.absoluteDir().path().toLocal8Bit().data();
    this->filename = F.fileName().toLocal8Bit().data();
    QDir Dir(F.absoluteDir());
    if(Dir.exists() == false)
        Dir.mkpath(Dir.absolutePath());
    QString CSSFileName = Dir.absolutePath()+"/style.css";
    if(BuildCSS(CSSFileName.toLocal8Bit().data()) == false) return false;

    fout.clear();
    fout.open(filename, ios::out | ios::trunc);
    if(!fout)
    {
        return false;
    };
    fout<<"<html>"<<endl;
    fout<<"<head>"<<endl;
    fout<<"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />"<<endl;
    fout<<"<link href=\"style.css\" rel=\"stylesheet\" type=\"text/css\" media=\"screen\" />"<<endl;
    fout<<"</head>"<<endl;
    fout<<"<body>"<<endl;
    created_files.clear();
    created_files.push_back(string(F.absoluteFilePath().toLocal8Bit().data()));
    created_files.push_back(string(CSSFileName.toLocal8Bit().data()));
    return true;
}

void THTMLWriter::EndWrite()
{
    fout<<"</body></html>"<<endl;
    fout.close();
}

void THTMLWriter::visit(const TFormulaPlotter& element) {
    AddInlineFormula(element);
    fout<<endl;
}

void THTMLWriter::visit(const THSpace& element) {
    Q_UNUSED(element);
    //nothing to do
}

void THTMLWriter::visit(const TLine& Line) {
    fout<<"<p>";
    for(size_t i = 0; i < Line.size(); i++)
        WriteRectangleElement(Line[i]);
    fout<<"</p>"<<endl;
}

void THTMLWriter::visit(const TLines& Lines) {
    fout<<"<div style=\"padding: 20px; background-color: #FFCCCC;\">";
    for(size_t i = 0; i < Lines.size(); i++)
        WriteRectangleElement(Lines.at(i));
    fout<<"</div>";
}

void THTMLWriter::visit(const TText& element) {
    AddParagraph(element.Text);
}


void THTMLWriter::WriteRectangleElement(const TRectangleElement *R)
{
    R->accept(*this);
}

void THTMLWriter::WriteError(string Message)
{    
    fout<<"<div class=\"error\">"<<Translate(Message)<<"</div>";
}
void THTMLWriter::BeginError()
{
    BeginDiv("error");
}

void THTMLWriter::EndError()
{
    EndDiv();
}

void THTMLWriter::AddHeader1(std::string str)
{
    fout<<"<h1>"<<Translate(str)<<"</h1>"<<endl;
}


void THTMLWriter::AddParagraph(string str)
{
    fout<<"<p>"<<Translate(str)<<"</p>"<<endl;
}
void THTMLWriter::AddParagraph(std::string str, int N)
{
    vector<const void*> V;
    V.push_back(&N);
    AddParagraph(str, V);
}

void THTMLWriter::AddParagraph(std::string str, const vector<const void*>& P)
{
    str = Translate(str);
    size_t startindex = 0;
    size_t len = str.length();
    size_t count = 0;
    BeginParagraph();
    for(size_t curindex = 0; curindex < len; curindex++)
    {
        if(str[curindex] == '\\')
            curindex++;
        if(str[curindex] == '%')
        {
            string SubStr = str.substr(startindex, curindex - startindex);
            fout<<SubStr<<endl;
            curindex++;
            if(curindex >= len) break;
            char code = str[curindex];
            if(P.size() <= count)
            {
                cerr<<"THTMLWriter::AddParagraph(string, vector): P.size() <= count, check format string, str = "<<str<<endl;
                throw "THTMLWriter::AddParagraph(string, vector): P.size() <= count, check format string str";
            };
            switch(code)
            {
            case 'd':
            case 'D':
            {
                stringstream str;
                const void *Pointer = P[count];
                int i = *(reinterpret_cast<const int*> (Pointer));
                fout<<i;
                break;
            };
            case 'n':
            {
                const TRectangleElement *R = reinterpret_cast<const TRectangleElement*> (P[count]);
                WriteRectangleElement(R);
                break;
            }
            case 'N':
            {
                const TRectangleElement *R = reinterpret_cast<const TRectangleElement*> (P[count]);
                NewLine();
                WriteRectangleElement(R);
                NewLine();
                break;
            }
            default:
                cerr<<"THTMLWriter::AddParagraph(string, vector): unknown format letter, check format string, str = "<<str<<endl;
                throw "THTMLWriter::AddParagraph(string, vector): unknown format letter, check format string str";
                break;

            }
            count++;
            startindex = curindex+1;
        }
    }
    string SubStr = str.substr(startindex);
    fout<<SubStr<<endl;
    EndParagraph();
}

void THTMLWriter::AddParagraph(std::string str, void* R)
{
    vector<const void*> V;
    V.push_back(R);
    AddParagraph(str, V);
}

void THTMLWriter::AddParagraph(std::string str, void* R1, void* R2)
{
    vector<const void*> V;
    V.push_back(R1);
    V.push_back(R2);
    AddParagraph(str, V);
}

void THTMLWriter::AddParagraph(std::string str, void* R1, void* R2, void* R3)
{
    vector<const void*> V;
    V.push_back(R1);
    V.push_back(R2);
    V.push_back(R3);
    AddParagraph(str, V);
}
void THTMLWriter::AddParagraph(std::string str, void* R1, void* R2, void* R3, void* R4)
{
    vector<const void*> V;
    V.push_back(R1);
    V.push_back(R2);
    V.push_back(R3);
    V.push_back(R4);
    AddParagraph(str, V);
}

void THTMLWriter::AddParagraph(std::string str, const TNumeric& N)
{
    vector<const void*> V;
    V.push_back(&N);
    AddParagraph(str, V);
}
void THTMLWriter::AddParagraph(std::string str, const TNumeric& N1, const TNumeric& N2)
{
    vector<const void*> V;
    V.push_back(&N1);
    V.push_back(&N2);
    AddParagraph(str, V);
}
void THTMLWriter::AddParagraph(std::string str, const TNumeric& N1, const int &D)
{
    vector<const void*> V;
    V.push_back(&N1);
    V.push_back(&D);
    AddParagraph(str, V);
}

void THTMLWriter::AddParagraph(std::string str, const TNumeric& N1, const TNumeric& N2, const TNumeric& N3)
{
    vector<const void*> V;
    V.push_back(&N1);
    V.push_back(&N2);
    V.push_back(&N3);
    AddParagraph(str, V);
}
void THTMLWriter::AddParagraph(std::string str, const TNumeric& N1, const TNumeric& N2, const TNumeric& N3, const TNumeric& N4)
{
    vector<const void*> V;
    V.push_back(&N1);
    V.push_back(&N2);
    V.push_back(&N3);
    V.push_back(&N4);
    AddParagraph(str, V);
}


void THTMLWriter::Add(string str)
{
    fout<<Translate(str)<<endl;
}

void THTMLWriter::BeginSubSection()
{
    fout<<"<div style=\"padding: 20px\">";
}

void THTMLWriter::EndSubSection()
{
    fout<<"</div>";
}

void THTMLWriter::BeginDiv(string DivName)
{
    if(DivName.empty()) fout<<"<div>"<<endl;
    else fout<<"<div class=\""<<DivName<<"\">"<<endl;
}

void THTMLWriter::EndDiv()
{
    fout<<"</div>"<<endl;
}

void THTMLWriter::BeginParagraph()
{
    fout<<"<p>";
}

void THTMLWriter::EndParagraph()
{
    fout<<"</p>"<<endl;
}

void THTMLWriter::NewLine()
{
    fout<<"<br/>"<<endl;
}

void THTMLWriter::IncrementNestingLevel()
{
    NestingLevel++;
    BeginDiv("nested");
}

void THTMLWriter::DecrementNestingLevel()
{
    NestingLevel--;
    EndDiv();
}


class TNumericExporter //: public QThread
{
public:
    TNumericExporter(TFormulaPlotter& formula_plotter) : fp(formula_plotter) {}
    TFormulaPlotter& fp;
    string FullFileName;
    QColor EditableColor, TextColor, FormulaColor;
    QFont FormulaFont, TextFont;
    void run();
};

void TNumericExporter::run()
{
    QPicture Pic;
    QPainter Painter;
    Painter.begin(&Pic);
    TPaintCanvas C(&Painter);
    C.EditableColor = EditableColor;
    C.TextColor = TextColor;
    C.FormulaColor = FormulaColor;
    C.FormulaFont = FormulaFont;
    C.TextFont = TextFont;
    int W, H, D;
    fp.GetTextRectangle(&C, W, H, D);
    fp.DrawAtBaseLeft(&C, 0, 0);
    Painter.end();
    QRect Rect(0, -H, W, H + D);
    QSize Size = Rect.size();
    QImage Im(Size, QImage::Format_ARGB32_Premultiplied); //(Size, QImage::Format_RGB888);
    Im.fill(qRgba(0, 0, 0, 0));
    QPainter Painter2;
    if(Painter2.begin(&Im))
    {
        //Painter2.fillRect(0, 0, Rect.width(), Rect.height(), QColor(0x80, 0x80, 0x80, 0x80));
        //Painter2.drawRect(0, 0, Rect.width()-1, Rect.height()-1);
        Painter2.drawPicture(-Rect.topLeft(), Pic);
        Painter2.end();
    } else {
        cerr<<"THTMLWriter::AddInlineFormula: the size of image is zero"<<endl;
        fp.GetTextRectangle(&C, W, H, D); //for debug purposes only
    }

    Im.save(QString::fromLocal8Bit(FullFileName.c_str()));
}


void THTMLWriter::AddInlineFormula(TFormulaPlotter fp)
{
    string FileName = GetUniqueFileName(Path.c_str(), "png");
    QFileInfo FileInfo(QString::fromLocal8Bit(Path.c_str()), QString::fromLocal8Bit(FileName.c_str()));
    string AbsoluteFilePath = FileInfo.absoluteFilePath().toLocal8Bit(  ).data();
    created_files.push_back(AbsoluteFilePath);
    TNumericExporter *W = new TNumericExporter(fp);
    W->FullFileName = AbsoluteFilePath;
    W->FormulaColor = FormulaColor;
    W->EditableColor = EditableColor;
    W->TextColor = TextColor;
    W->FormulaFont = FormulaFont;
    W->TextFont = TextFont;
    W->run();

    stringstream padstr;
    padstr<<"style=\"vertical-align: middle;\"";  
    //fout<<"<img src=\""<<FileName<<"\" align = \"bottom\" "<<padstr.str()<<" alt = \""<<N.CodeBasic()<<"\"/>";
    fout<<"<img src=\""<<FileName<<"\" align = \"bottom\" "<<padstr.str()<<" alt = \""<<fp.CodeBasic()<<"\"/>";
    /*fout<<"some text";
    fout<<"<img src=\""<<FileName<<"\" align = \"middle\" "<<padstr.str()<<" alt = \""<<N.CodeBasic()<<"\"/>";*/
}

void THTMLWriter::AddFormula(TFormulaPlotter fp)
{
    fout<<"<br/>"<<endl;
    AddInlineFormula(fp);
    fout<<"<br/>"<<endl;
}

void THTMLWriter::AddFormula(const TNumeric& N)
{
    AddFormula(TFormulaPlotter(N));
}
void THTMLWriter::AddInlineFormula(const TNumeric& N) {
    AddInlineFormula(TFormulaPlotter(N));
}


string THTMLWriter::GetUniqueFileName(const char* Path, const char* ext)
{
    Q_UNUSED(Path);
    UniqueFileCounter++;
    stringstream str;
    str<<UniqueFileCounter<<"."<<ext;
    return str.str();
}

string THTMLWriter::GetFullPath()
{
    return Path+"/"+filename;
}
string THTMLWriter::GetFolder()
{
    return Path;
}
string THTMLWriter::GetFileName()
{
    return filename;
}


void THTMLWriter::PushTranslator(TMyTranslator* T)
{
    Translators.push_back(T);
}

void THTMLWriter::PopTranslator()
{
    if(Translators.size() > 0)Translators.pop_back();
}
string THTMLWriter::Translate(string Str)
{
    if(Translators.size() == 0) return Str;
    size_t i = Translators.size();
    string res;
    do
    {
        i--;
        res = Translators[i]->tr(Str);
        if(Translators[i]->Status)
            return res;
    }while(i>0);
    return Str;
}
