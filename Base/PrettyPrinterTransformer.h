#ifndef PRETTY_PRINTER_TRANSFORMER_
#define PRETTY_PRINTER_TRANSFORMER_

#include "arithmetic.h"

// convert formula to nice representation before printing
class PrettyPrinterTransformer {
    public:        
        static PNumeric convert(const PNumeric numeric);
    private:
        static PNumeric convertSum(const PNumeric numeric);
};

#endif // PRETTY_PRINTER_TRANSFORMER_