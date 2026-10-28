#include "Symbol_table.h"

/*
    get — вернуть значение величины с именем s.
    Поиск линейный (по всему вектору) — просто и достаточно для
    небольшого числа величин. Если имя не объявлено — ошибка.
*/
double Symbol_table::get(string s) const
{
    for (size_t i = 0; i < var_table.size(); ++i)
        if (var_table[i].name == s)
            return var_table[i].value;
    error("get: undefined name ", s); // имя не найдено
}

/*
    set — присвоить величине s значение d (упр. 2: оператор «=»).
    Ошибки: имя не объявлено; величина — константа (упр. 3).
*/
void Symbol_table::set(string s, double d)
{
    for (size_t i = 0; i < var_table.size(); ++i)
        if (var_table[i].name == s)
        {
            if (var_table[i].is_const)
                error(s, " is a constant");
            var_table[i].value = d;
            return;
        }
    error("set: undefined name ", s); // имя не найдено
}

/*
    is_declared — объявлена ли величина с именем s?
    Используется в declaration(), чтобы запретить повторное объявление.
*/
bool Symbol_table::is_declared(string s) const
{
    for (size_t i = 0; i < var_table.size(); ++i)
        if (var_table[i].name == s)
            return true;
    return false;
}

/*
    define — объявить величину s со значением d (и флагом константы).
    Повторное объявление — ошибка.
*/
void Symbol_table::define(string s, double d, bool is_const)
{
    if (is_declared(s))
        error(s, " declared twice");
    var_table.push_back(Variable(s, d, is_const));
}
