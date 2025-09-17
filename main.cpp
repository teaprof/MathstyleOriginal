//todo: Solve failed for equation space #1
#include <QApplication>
#include <QTranslator>
#include "mainwindow.h"
#include <mathomatic.h>

#include "Base/arithmetic.h"
#include <iostream>
#include <fstream>
#include <string.h>

QString ApplicationDirPath;
QApplication* qAppl;

#include "Base/arithmetic.h"
#include "Base/TNumericMask.h"
#include "Base/polynom.h"

void TestFactorise()
{
    init_mathomatic();
TPolynom P1, P2;
    P1.Coef.push_back(TNumeric(16));
    P1.Coef.push_back(TNumeric(0));
    P1.Coef.push_back(TNumeric(1));
    P2.Coef.push_back(TNumeric(27));
    P2.Coef.push_back(TNumeric(0));
    P2.Coef.push_back(TNumeric(1));

TPolynom P = P1*P1;
    P.Coef.clear();
    P.Coef.push_back(TNumeric(3));
    P.Coef.push_back(TNumeric(0));
    P.Coef.push_back(TNumeric(17));
    P.Coef.push_back(TNumeric(0));
    P.Coef.push_back(TNumeric(10));

    cout<<"Source: "<<P.GetNumeric(TNumeric("x")).CodeBasic()<<endl;

vector<TPolynom> F = P.FactorizeKroneker();
    for(size_t i = 0; i < F.size(); i++)
        cout<<F[i].GetNumeric(TNumeric("x")).CodeBasic()<<endl;
    done_mathomatic();
}

void TestMask()
{
    init_mathomatic();
TNumericMask Mask;
TNumeric Template, N;
    //Template.Assign("(m*x+c)/((a*x^2+b*x+c)^2)");
    //N.Assign("1/((x^2+2*x+1)^2)");
    Template.Assign("b*x+c");
    N.Assign("2*(1-x)");
    //Template = TNumeric("a")*MakeSin(MakeProd(TNumeric("b"), TNumeric("x")));
    //N = MakeSin(MakeProd(TNumeric("0"), TNumeric("x")));
    bool res = Mask.Match(Template, N);
    if(res)
    {
        cout<<"Match is found."<<endl;
        for(map<string, TNumeric>::iterator it = Mask.Coefs.begin(); it != Mask.Coefs.end(); it++)
        {
            cout<<it->first<<" = "<<it->second.CodeBasic()<<endl;
        }
    } else cout<<"Does not match."<<endl;
    done_mathomatic();
};

void TestMask2()
{
    init_mathomatic();
    TNumeric N;
    N.Assign("sin(x+2)*exp(2*x^2+b*cos(x))");
    cout<<N.CodeBasic()<<endl;
    done_mathomatic();
};

QTranslator* Tr;

int main(int argc, char *argv[])
{
    try
    {
        init_mathomatic();

        Tr = new QTranslator();
        Tr->load("en2ru");



        QApplication a(argc, argv);
        qAppl = &a;
        a.installTranslator(Tr);
        ApplicationDirPath = a.applicationDirPath();
        MainWindow w;
        w.show();

        int Res = a.exec();
        done_mathomatic();
        delete Tr;
        return Res;
        }catch(const char* s)
        {
            cerr<<"Exception handled: "<<s<<endl;
            return -1;
        } catch (...)
        {
            cerr<<"Unknown exception handled"<<endl;
            return -1;
        }
}
