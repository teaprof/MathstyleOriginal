#ifndef TINTERVAL_H
#define TINTERVAL_H
#include<vector>
using namespace std;
#include "arithmetic.h"

extern TNumeric NumericMinusInf;
extern TNumeric NumericPlusInf;


class TInterval
{
    void Assign(const TInterval& I);
public:
    //Точка: левая и правая граница совпадают, обе границы включены во множество
    bool Empty; //если Empty, то это пустое множество и Left, Right, IncludeLeft, IncludeRight не учитываются
    TNumeric Left, Right; //левая и правая границы
    bool IncludeLeft; //входит ли левая граница во множество
    bool IncludeRight; //входит ли правая граница во множество
    TInterval();
    TInterval(const TNumeric& Value); //создает отрезок [Value, Value]
    TInterval(const TInterval& I);
    TInterval(const TNumeric& Left, const TNumeric& Right, bool IncludeLeft = false, bool IncludeRight = false);
    bool TestPoint(const TNumeric& P) const; //истина, если точка принадлежит интервалу
    bool operator<(const TInterval& I) const; //true, если самая левая точка интевала *this левее самой левой точки I

    bool operator==(const TInterval& I) const;

    TNumeric GetNumeric();

    TInterval operator*(const TInterval& I) const; //finds the intersection of *this and I
};

class TIntervalsSet
{
        void Assign(const TIntervalsSet& Set);
        //vector<TInterval> GetIncidentIntervals(const TInterval& I); //находит интервалы, имеющие с I хотя бы одну общую точку

        //size_t GetMostLeftInterval(const TNumeric& MaxBound, bool MinBoundInclude);
        //находит номер интервала с наименьшим значением левой границы, но не меньшей или строго большей, чем MinBound
        //возвращает Intervals.size(), если такого элемента нет
/*        size_t GetMostRightInterval(const TNumeric& MaxBound, bool MinBoundInclude);
        //находит номер интервала с наименьшим значением левой границы, но не меньшей или строго большей, чем MinBound
        //возвращает Intervals.size(), если такого элемента нет*/
        void Simplify(); //сортирует по левому концу и объединяет пересекающиеся интервалы
    public:
        vector<TInterval> Intervals;
        TIntervalsSet();
        TIntervalsSet(const TIntervalsSet& Set);
        TIntervalsSet(const TInterval& I);
        ~TIntervalsSet();

        TIntervalsSet operator=(const TIntervalsSet& Set);
        TIntervalsSet operator+(const TIntervalsSet& Set); //find the union of *this and set
        TIntervalsSet operator*(const TIntervalsSet& Set); //find the intersection of *this and set
        TIntervalsSet operator-(const TInterval& I);
        TIntervalsSet operator-(const TIntervalsSet& Set);
        TNumeric GetNumeric();

        bool TestPoint(const TNumeric& P) const; //истина, если точка принадлежит интервалам
};

TIntervalsSet operator+(const TInterval& I1, const TInterval& I2);
TIntervalsSet operator-(const TInterval& I1, const TInterval& I2);

#endif // TINTERVAL_H
