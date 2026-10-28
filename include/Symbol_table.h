#pragma once

#include "std_lib_facilities.h"

/*
    Variable — именованная величина калькулятора:
    имя (name), значение (value) и флаг, отличающий константу
    от переменной (упр. 3: константу нельзя менять присваиванием).
*/
struct Variable
{
    string name;   // имя величины
    double value;  // значение
    bool is_const; // true — константа, false — переменная
    Variable(string n, double v, bool c = false)
        : name(n), value(v), is_const(c)
    {
    }
};

/*
    Symbol_table — «таблица символов»: хранит все объявленные
    величины (упр. 4: заменяет глобальный vector<Variable> names
    и функции get_value()/set_value()/is_declared()).
    var_table — единственное хранилище, доступное только классу.
*/
class Symbol_table
{
    vector<Variable> var_table; // все объявленные величины

  public:
    double get(string s) const; // значение s; если не объявлена — ошибка
    void
    set(string s,
        double d); // присвоение s; если не объявлена или константа — ошибка
    bool is_declared(string s) const;                       // объявлена ли s?
    void define(string s, double d, bool is_const = false); // объявить s
};
