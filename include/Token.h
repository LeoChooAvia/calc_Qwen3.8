#pragma once

#include "std_lib_facilities.h"

/*
    Token — токен: часть входного текста, осмысленная для анализатора.
    kind  — тип токена (символ-оператор либо служебный код из
   SymbolicConstants.h); value — числовое значение (используется, когда kind ==
   number); name  — строка (используется, когда kind == name).
*/
struct Token
{
    char kind;
    double value;
    string name;

    Token(char ch);
    Token(char ch, double val);
    Token(char ch, string n);
};
