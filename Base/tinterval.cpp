#include <algorithm>
using namespace std;
#include "tinterval.h"
TNumeric NumericPlusInf("\\infty");
TNumeric NumericMinusInf("-\\infty");

TInterval::TInterval() {
    Left = TNumeric(0);
    Right = TNumeric(1);
    IncludeLeft = IncludeRight = false;
    Empty = false;
}
TInterval::TInterval(const TInterval& I) {
    Assign(I);
}

TInterval::TInterval(const TNumeric& Left, const TNumeric& Right, bool IncludeLeft, bool IncludeRight) {
    this->Left = Left;
    this->Right = Right;
    this->IncludeLeft = IncludeLeft;
    this->IncludeRight = IncludeRight;
    this->Empty = false;
}

TInterval::TInterval(const TNumeric& Value) {
    this->Left = Value;
    this->Right = Value;
    this->IncludeLeft = true;
    this->IncludeRight = true;
    this->Empty = false;
}

void TInterval::Assign(const TInterval& I) {
    Left = I.Left;
    Right = I.Right;
    IncludeLeft = I.IncludeLeft;
    IncludeRight = I.IncludeRight;
    this->Empty = I.Empty;
}

bool TInterval::operator<(const TInterval& I) const {
    if (Empty)
        return false;
    if (Left.Calculate() < I.Left.Calculate())
        return true;
    if (Left.Calculate() == I.Left.Calculate())
        if (IncludeLeft && !IncludeRight)
            return true;
    return false;
}

TInterval TInterval::operator=(const TInterval& I) {
    Assign(I);
    return *this;
}
bool TInterval::operator==(const TInterval& I) const {
    if (Left == I.Left && Right == I.Right && IncludeLeft == I.IncludeLeft && IncludeRight == I.IncludeRight)
        return true;
    else
        return false;
}

TNumeric TInterval::asNumeric() {
    TNumeric Res;
    if (Empty) {
        Res = MakeInline(TNumeric("{"), TNumeric("\\empty"), TNumeric("}"));
    } else {
        if (Left != Right) {
            if (IncludeLeft && IncludeRight)
                Res.operation = OperatorSegment;
            if (!IncludeLeft && IncludeRight)
                Res.operation = OperatorIntervalSegment;
            if (IncludeLeft && !IncludeRight)
                Res.operation = OperatorSegmentInterval;
            if (!IncludeLeft && !IncludeRight)
                Res.operation = OperatorInterval;

            Res.OperandsPushback(Left);
            Res.OperandsPushback(Right);
        } else {
            if (IncludeLeft && IncludeRight) {
                Res = MakeInline(TNumeric("{"), Left, TNumeric("}"));
            } else {
                Res = MakeInline(TNumeric("{"), TNumeric("\\empty"), TNumeric("}"));
            }
        }
    };
    return Res;
}

bool TInterval::TestPoint(const TNumeric& P) const {
    if (Empty)
        return false;
    double p = P.Calculate();
    double l = Left.Calculate();
    double r = Right.Calculate();
    if (p >= l && p <= r) {
        if (p > l && p < r)
            return true;
        else if (p == l && p != r)
            return IncludeLeft;
        else if (p != l && p == r)
            return IncludeRight;
        else
            // то есть if(p == l && p == r)
            return (IncludeLeft && IncludeRight);
    };
    return false;
}

TInterval TInterval::operator*(const TInterval& I) const {
    TInterval Res;
    if (Empty || I.Empty) {
        Res.Empty = true;
        return Res;
    }
    if (I.Left.Calculate() > Right.Calculate() || I.Right.Calculate() < Left.Calculate())
        Res.Empty = true;
    if (I.Left.Calculate() == Right.Calculate()) {
        if (I.IncludeLeft && IncludeRight) {
            Res.Left = Right;
            Res.Right = Right;
            Res.IncludeLeft = Res.IncludeRight = true;
        } else {
            Res.Empty = true;
        }
    }
    if (Left.Calculate() == I.Right.Calculate()) {
        if (IncludeLeft && I.IncludeRight) {
            Res.Left = Left;
            Res.Right = Left;
            Res.IncludeLeft = Res.IncludeRight = true;
        } else {
            Res.Empty = true;
        }
    }
    if (Left.Calculate() > I.Left.Calculate()) {
        Res.IncludeLeft = IncludeLeft;
        Res.Left = Left;
    };
    if (Left.Calculate() < I.Left.Calculate()) {
        Res.IncludeLeft = I.IncludeLeft;
        Res.Left = I.Left;
    };
    if (Left.Calculate() == I.Left.Calculate()) {
        Res.IncludeLeft = IncludeLeft && I.IncludeLeft;
        Res.Left = Left;
    };
    if (Right.Calculate() < I.Right.Calculate()) {
        Res.IncludeRight = IncludeRight;
        Res.Right = Right;
    };
    if (Right.Calculate() > I.Right.Calculate()) {
        Res.IncludeRight = I.IncludeRight;
        Res.Right = I.Right;
    };
    if (Right.Calculate() == I.Right.Calculate()) {
        Res.IncludeRight = I.IncludeRight && IncludeRight;
        Res.Right = I.Right;
    };
    return Res;
}

TIntervalsSet operator-(const TInterval& I1, const TInterval& I2)
// finds the substraction of I1 and I2

{
    if (I1.Empty)
        return TIntervalsSet(I1);
    if (I2.Empty)
        return TIntervalsSet(I1);
    if ((I1 * I2).Empty)
        return TIntervalsSet(I1);

    // оба интервала не пустых, и пересекаются
    // 1. Случай, когда I2 целиком внутри I1
    if (I1.TestPoint(I2.Left) && I1.TestPoint(I2.Right)) {
        TInterval Res1(I1.Left, I2.Left, I1.IncludeLeft, !I2.IncludeLeft);
        if (Res1.Left.Calculate() == Res1.Right.Calculate())
            if (Res1.IncludeLeft == false || Res1.IncludeRight == false)
                Res1.Empty = true;

        TInterval Res2(I2.Right, I1.Right, !I2.IncludeRight, I1.IncludeRight);
        if (Res2.Left.Calculate() == Res2.Right.Calculate())
            if (Res2.IncludeLeft == false || Res2.IncludeRight == false)
                Res2.Empty = true;

        TIntervalsSet I = Res1 + Res2;
        return I;
    };

    // Случай, когда I1 целиком внутри I2
    if (I2.TestPoint(I1.Left) && I2.TestPoint(I1.Right))
        return TIntervalsSet();  // получается пустое множество

    // Оставшийся случай:
    // Случай, когда I1 и I2 пересекаются, и их симметрическая разность не пуста
    TInterval Res = I1;
    if (Res.TestPoint(I2.Left)) {
        Res.Right = I2.Left;
        Res.IncludeRight = !I2.IncludeLeft;
    };
    if (Res.TestPoint(I2.Right)) {
        Res.Left = I2.Right;
        Res.IncludeLeft = !I2.IncludeRight;
    }
    if (Res.Left.Calculate() < Res.Right.Calculate())
        Res.Empty = true;
    if (Res.Left.Calculate() == Res.Right.Calculate())
        if (Res.IncludeLeft == false || Res.IncludeRight == false)
            Res.Empty = true;
    return TIntervalsSet(Res);
}

TIntervalsSet operator+(const TInterval& I1, const TInterval& I2) {
    return TIntervalsSet(I1) + I2;
}

void TIntervalsSet::Assign(const TIntervalsSet& Set) {
    Intervals.clear();
    for (size_t i = 0; i < Set.Intervals.size(); i++)
        Intervals.push_back(Set.Intervals[i]);
}

TIntervalsSet::TIntervalsSet() {}
TIntervalsSet::TIntervalsSet(const TIntervalsSet& Set) {
    Assign(Set);
}

TIntervalsSet::TIntervalsSet(const TInterval& I) {
    Intervals.push_back(I);
}

TIntervalsSet::~TIntervalsSet() {}

TIntervalsSet TIntervalsSet::operator=(const TIntervalsSet& Set) {
    Assign(Set);
    return *this;
}

/*size_t TIntervalsSet::GetMostLeftInterval(const TNumeric& MinBound, bool MinBoundInclude)
{
size_t n = Intervals.size();
    for(size_t i = 0; i < Intervals.size(); i++)
    {
        bool Satisfy = false;
        if(MinBoundInclude)
            if(Intervals[i].Left.Calculate() >= MinBound.Calculate()) Satisfy = true;
        if(!MinBoundInclude)
            if(Intervals[i].Left.Calculate() > MinBound.Calculate()) Satisfy = true;
        if(n == Intervals.size() || Intervals[i].Left.Calculate() < Intervals[n].Left.Calculate())
            n = i;
    }
}*/

void TIntervalsSet::Simplify() {
    // удаляем все пустые интервалы
    size_t i = 0;
    while (i < Intervals.size())
        if (Intervals[i].Empty)
            Intervals.erase(Intervals.begin() + i);
        else
            i++;
    // если ничего не осталось, то сразу завершаемся
    if (Intervals.empty())
        return;

    // сортируем по левому концу
    sort(Intervals.begin(), Intervals.end());
    i = 0;
    while (i < Intervals.size() - 1) {
        // проверяем, касается i-ый интервал i+1-ого
        bool Touches = false;
        if (Intervals[i].Right.Calculate() > Intervals[i + 1].Left.Calculate())
            Touches = true;
        if (Intervals[i].Right.Calculate() == Intervals[i + 1].Left.Calculate()) {
            if (Intervals[i].IncludeRight == true || Intervals[i + 1].IncludeLeft == true)
                Touches = true;
        }

        if (Touches) {
            // проводим объединение интервалов
            // if(Intervals[i].Right.Calculate() > Intervals[i+1].Right.Calculate()); -- ничего не делаем в этом случае
            if (Intervals[i].Right.Calculate() < Intervals[i + 1].Right.Calculate()) {
                Intervals[i].Right = Intervals[i + 1].Right;
                Intervals[i].IncludeRight = Intervals[i + 1].IncludeRight;
            };
            if (Intervals[i].Right.Calculate() == Intervals[i + 1].Right.Calculate()) {
                Intervals[i].IncludeRight = Intervals[i].IncludeRight || Intervals[i + 1].IncludeRight;
            }
            Intervals.erase(Intervals.begin() + i + 1);
        } else {
            // i-ый интервал больше ни с чем не пересекается, переходим ко следующему
            i++;
        }
    }
}

TIntervalsSet TIntervalsSet::operator+(const TIntervalsSet& Set) {
    TIntervalsSet Res(*this);
    Res.Intervals.insert(Res.Intervals.end(), Set.Intervals.begin(),
                         Set.Intervals.end());  // Res содержит все интевалы из *this и из Set

    Res.Simplify();
    return Res;
}

TIntervalsSet TIntervalsSet::operator*(const TIntervalsSet& Set) {
    TIntervalsSet Res;
    for (size_t i = 0; i < Intervals.size(); i++)
        for (size_t j = 0; j < Set.Intervals.size(); j++) {
            Res.Intervals.push_back(Intervals[i] * Set.Intervals[j]);
        };
    Res.Simplify();
    return Res;
}

TIntervalsSet TIntervalsSet::operator-(const TInterval& I) {
    TIntervalsSet Res;
    for (size_t i = 0; i < Intervals.size(); i++) {
        TIntervalsSet NewI = Intervals[i] - I;
        Res = Res + NewI;
    }
    Res.Simplify();
    return Res;
}

TIntervalsSet TIntervalsSet::operator-(const TIntervalsSet& I) {
    TIntervalsSet Res;
    if (I.Intervals.size() == 0)
        Res = *this;
    else {
        for (size_t i = 0; i < I.Intervals.size(); i++)
            Res = Res + (*this - I.Intervals[i]);
        Res.Simplify();
    };
    return Res;
}

bool TIntervalsSet::TestPoint(const TNumeric& P) const {
    for (size_t i = 0; i < Intervals.size(); i++)
        if (Intervals[i].TestPoint(P))
            return true;
    return false;
}

TNumeric TIntervalsSet::asNumeric() {
    TNumeric Res;

    if (Intervals.size() == 0)
        Res = MakeInline(TNumeric("{"), TNumeric("\\empty"), TNumeric("}"));
    else {
        Res.operation = OperatorUnion;
        for (size_t i = 0; i < Intervals.size(); i++)
            Res.OperandsPushback(Intervals[i].asNumeric());
    };
    return Res;
}
