// todo: Solve failed for equation space #1
#include <QApplication>
#include <QTranslator>

#include <fstream>
#include <iostream>
#include <mathomatic.h>
#include <string.h>

#include "Base/arithmetic.h"
#include "mainwindow.h"

QString ApplicationDirPath;
QApplication* qAppl;

#include "Base/TNumericMask.h"
#include "Base/arithmetic.h"
#include "Base/polynom.h"

void TestFactorise() {
    init_mathomatic();
    TPolynom P1, P2;
    P1.Coef.push_back(TNumeric::create(16));
    P1.Coef.push_back(TNumeric::create(0));
    P1.Coef.push_back(TNumeric::create(1));
    P2.Coef.push_back(TNumeric::create(27));
    P2.Coef.push_back(TNumeric::create(0));
    P2.Coef.push_back(TNumeric::create(1));

    TPolynom P = P1 * P1;
    P.Coef.clear();
    P.Coef.push_back(TNumeric::create(3));
    P.Coef.push_back(TNumeric::create(0));
    P.Coef.push_back(TNumeric::create(17));
    P.Coef.push_back(TNumeric::create(0));
    P.Coef.push_back(TNumeric::create(10));

    cout << "Source: " << P.asNumeric(TNumeric("x"))->CodeBasic() << endl;

    vector<TPolynom> F = P.FactorizeKroneker();
    for (size_t i = 0; i < F.size(); i++)
        cout << F[i].asNumeric(TNumeric("x"))->CodeBasic() << endl;
    done_mathomatic();
}

void TestMask() {
    init_mathomatic();
    TNumericMask Mask;
    TNumeric Template, N;
    // Template.Assign("(m*x+c)/((a*x^2+b*x+c)^2)");
    // N.Assign("1/((x^2+2*x+1)^2)");
    Template.Assign("b*x+c");
    N.Assign("2*(1-x)");
    // Template = TNumeric("a")*MakeSin(MakeProd(TNumeric("b"), TNumeric("x")));
    // N = MakeSin(MakeProd(TNumeric("0"), TNumeric("x")));
    bool res = Mask.Match(Template, N);
    if (res) {
        cout << "Match is found." << endl;
        for (map<string, TNumeric>::iterator it = Mask.Coefs.begin(); it != Mask.Coefs.end(); it++) {
            cout << it->first << " = " << it->second.CodeBasic() << endl;
        }
    } else
        cout << "Does not match." << endl;
    done_mathomatic();
};

void TestMask2() {
    init_mathomatic();
    TNumeric N;
    N.Assign("sin(x+2)*exp(2*x^2+b*cos(x))");
    cout << N.CodeBasic() << endl;
    done_mathomatic();
}

QTranslator* Tr;

int main(int argc, char* argv[]) {
    try {
        init_mathomatic();

        Tr = new QTranslator();
        if (Tr->load("en2ru") == false) {
            std::cout << "Can't load dictionary" << std::endl;
        }

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
    } catch (const char* s) {
        cerr << "Exception handled: " << s << endl;
        return -1;
    } catch (...) {
        cerr << "Unknown exception handled" << endl;
        return -1;
    }
}
