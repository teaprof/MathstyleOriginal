#ifndef SYMBOLS_H
#define SYMBOLS_H
#include <string>
using namespace std;
void UniCode2UTF8(char *dest, int Code);
string UniCode2UTF8String(int Code);

string Recognize(string Str); //преобразует управляющие последовательности latex в UTF-8 коды соответствующих символов

//Some useful symbol codes
#define UnicodeElementOf 0x2208
#define UnicodeMiddleDot 0x22C5
#define UnicodeInfinity 0x221E
#define UnicodeEmpty 0x2205
#define UnicodeUnion 0x222A
#define UnicodeIntersection 0x2229
#define UnicodeLess 0x003C
#define UnicodeLessOrEqual 0x2264
#define UnicodeGreater 0x003E
#define UnicodeGreaterOrEqual 0x2265
#define UnicodePlusMinus 0x2213
#define UnicodePhi 0x03C6
#define UnicodePsi 0x03C8
#define UnicodePi 0x03C0
#define UnicodeEpsilon 0x03B5
#define UnicodeIntegral 0x222B

//библиотека кодов в Unicode UTF16
#define UnicodeFULLWIDTH_LEFT_PARENTHESIS  0xFF08
#define UnicodeFULLWIDTH_RIGHT_PARENTHESIS 0xFF09

#define UnicodeFULLWIDTH_LEFT_CURLY_BRACKET 0xFF5B
#define UnicodeFULLWIDTH_RIGHT_CURLY_BRACKET 0xFF5D

#define UnicodeFULLWIDTH_LEFT_SQUARE_BRACKET 0xFF3B
#define UnicodeFULLWIDTH_RIGHT_SQUARE_BRACKET 0xFF3D

#define UnicodeFULLWIDTH_PLUS_SIGN 0xFF0B
#define UnicodeFULLWIDTH_HYPHEN_MINUS 0xFF0D
#define UnicodeFULLWIDTH_LESS_THAN_SIGN  0xFF1C
#define UnicodeFULLWIDTH_GREATER_THAN_SIGN 0xFF1E
#define UnicodeLESS_THAN_OR_SLANTED_EQUAL_TO 0x2A7D
#define UnicodeGREATER_THAN_OR_SLANTED_EQUAL_TO 0x2A7E
#define UnicodeFULLWIDTH_EQUALS_SIGN 0xFF1D


//Эти значения используются при отрисовке
//Используем Юникод-расширение таблицы символов
//#define __USE_UNICODE__
#ifdef __USE_UNICODE__
#define MyLeftParenthesis UnicodeFULLWIDTH_LEFT_PARENTHESIS
#define MyRightParenthesis UnicodeFULLWIDTH_RIGHT_PARENTHESIS

#define MyLeftCurlyBracket UnicodeFULLWIDTH_LEFT_CURLY_BRACKET
#define MyRightCurlyBracket UnicodeFULLWIDTH_RIGHT_CURLY_BRACKET

#define MyLeftSquareBracket UnicodeFULLWIDTH_LEFT_SQUARE_BRACKET
#define MyRightSquareBracket UnicodeFULLWIDTH_RIGHT_SQUARE_BRACKET

#define MyPlusSign UnicodeFULLWIDTH_PLUS_SIGN
#define MyMinusSign UnicodeFULLWIDTH_HYPHEN_MINUS
#define MyLessThanSign  0x3C
#define MyGreaterThanSign 0x3E
#define MyLessOrEqualSign UnicodeLESS_THAN_OR_SLANTED_EQUAL_TO
#define MyGreaterOrEqualSign UnicodeGREATER_THAN_OR_SLANTED_EQUAL_TO
#define MyEqualSign 0x3D

#else

//Используем обычные символы
#define MyLeftParenthesis '('
#define MyRightParenthesis ')'

#define MyLeftCurlyBracket '{'
#define MyRightCurlyBracket '}'

#define MyLeftSquareBracket '['
#define MyRightSquareBracket ']'

#define MyPlusSign '+'
#define MyMinusSign '-'
#define MyLessThanSign  '<'
#define MyGreaterThanSign '>'
#define MyLessOrEqualSign UnicodeLessOrEqual
#define MyGreaterOrEqualSign UnicodeGreaterOrEqual
#define MyEqualSign '='

#endif

#endif // SYMBOLS_H
