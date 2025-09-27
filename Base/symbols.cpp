#include <Base/symbols.h>
void UniCode2UTF8(char* dest, int Code) {
    if (Code < 0x80)
        *dest++ = Code;
    else if (Code < 0x800)
        *dest++ = 192 + Code / 64, *dest++ = 128 + Code % 64;
    else if (Code - 0xd800u < 0x800)
        *dest = 0;
    else if (Code < 0x10000)
        *dest++ = 224 + Code / 4096, *dest++ = 128 + Code / 64 % 64, *dest++ = 128 + Code % 64;
    else if (Code < 0x110000)
        *dest++ = 240 + Code / 262144, *dest++ = 128 + Code / 4096 % 64, *dest++ = 128 + Code / 64 % 64, *dest++ = 128 + Code % 64;
    else
        *dest = 0;
    *dest = 0;
}

string UniCode2UTF8String(int Code) {
    char dest[10];
    UniCode2UTF8(dest, Code);
    return string(dest);
}

string Recognize(string Str, string SubStr, string Replacement) {
    size_t i;
    while ((i = Str.find(SubStr)) != string::npos) {
        Str.replace(i, SubStr.length(), Replacement);
    };
    return Str;
}

string Recognize(string Str)
// TODO: OPTIMIZE HERE
{
    Str = Recognize(Str, "\\infty", UniCode2UTF8String(UnicodeInfinity));
    Str = Recognize(Str, "\\empty", UniCode2UTF8String(UnicodeEmpty));
    Str = Recognize(Str, "\\phi", UniCode2UTF8String(UnicodePhi));
    Str = Recognize(Str, "\\psi", UniCode2UTF8String(UnicodePsi));
    Str = Recognize(Str, "\\pi", UniCode2UTF8String(UnicodePi));
    Str = Recognize(Str, "\\eps", UniCode2UTF8String(UnicodeEpsilon));
    return Str;
}
