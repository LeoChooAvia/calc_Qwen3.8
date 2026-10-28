/*
    Token — токен (глава 7):
    kind — «тип» токена: сам символ-оператор ('+', '-' и т.д.)
           или служебный код (number, name, let, quit, print —
           см. SymbolicConstants.h);
    value — числовое значение (для числовых литералов);
    name  — строковое имя (для имён переменных и ключевых слов).
*/

#include "Token.h"

Token::Token(char ch) : kind(ch), value(0) {};
Token::Token(char ch, double val) : kind(ch), value(val) {}
Token::Token(char ch, string n) : kind(ch), name(n) {}
