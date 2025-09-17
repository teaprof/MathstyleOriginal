#ifndef TMATRIX_H
#define TMATRIX_H

#include "arithmetic.h"

typedef TNumeric* PNumeric;

class TMatrix : public TNumeric
{
    PNumeric* Data;

    //y = 0, 1, .., Rows-1
    //x = 0, 1, ..., Cols-1
    TNumeric* at(size_t y, size_t x);
public:
    size_t Cols, Rows;
    TMatrix();
    ~TMatrix();

    void Resize(size_t NewCols, size_t NewRows);
};

#endif // TMATRIX_H
