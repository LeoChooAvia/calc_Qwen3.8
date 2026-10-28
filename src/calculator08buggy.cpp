/*
    Калькулятор (глава 7, Б. Страуструп,
    «Programming: Principles and Practice using C++»).
    Файл calculator08buggy.cpp — исходная версия программы.

    Ввод осуществляется из потока cin; вывод — в поток cout;
    сообщения об ошибках — в поток cerr.

    Усовершенствования (упр. 9) — три предложения:
      1. Оператор «^» (степень) — РЕАЛИЗОВАНО: приоритет выше, чем у
         * / %, ниже, чем у скобок; правоассоциативен (2^3^2 = 2^(3^2));
         реализован функцией pow() в factor() (см. ниже).
      2. Математические функции: sqrt, sin, cos, tan, log, exp —
         вызов вида sqrt(2); или sin(x);. Реализовать в primary():
         после чтения имени проверять, не имя ли функции, и при
         следующем токене «(» вычислять функцию от выражения в скобках.
      3. Команда выдачи всех переменных (например, «vars»): печатает
         имена и значения из Symbol_table — удобно после длинной сессии.
*/

#include "std_lib_facilities.h" // учебный заголовок: стандартные библиотеки + error()

#include "Symbol_table.h"      // класс Symbol_table — таблица символов (упр. 4)
#include "SymbolicConstants.h" // коды служебных токенов и строки prompt/result
#include "Token.h"             // структура Token — токен
#include "Token_stream.h"      // класс Token_stream — поток токенов

Symbol_table symtab; // таблица символов: все объявленные величины
Token_stream ts;     // единый источник токенов (читает cin, умеет unget)

double expression(); // переднее объявление: primary() и statement() вызывают
                     // expression()
double factor(); // переднее объявление: term() вызывает factor()

/*
    primary — «первичное» выражение, имеет наибольший приоритет:
        Число, ( expression ), имя переменной, - primary, + primary.
    Читает один токен из ts и обрабатывает его в зависимости от kind.
    Если токен не может начинать primary, бросает ошибку.
*/
double primary()
{
    Token t = ts.get();
    switch (t.kind)
    {
        case '(':
        {
            // ( expression ) — скобки задают порядок вычислений
            double d = expression();
            t = ts.get();
            if (t.kind != ')')
                error("')' expected");
            return d;
        }
        case '-':
            return -primary(); // унарный минус
        case '+':
            return +primary(); // унарный плюс
        case number:
            return t.value; // числовой литерал
        case name:
            return symtab.get(t.name); // обращение к переменной
        default:
            error("primary expected");
    }
}

/*
    term — произведение/частное/остаток, связывает factor операторами * / и %
    (приоритет выше, чем у + и -, ниже, чем у ^).
    left — уже разобранная левая часть (первый factor либо значение имени).
    Цикл читает оператор, и если он не *, / или %, возвращает токен
    назад (ts.unget) — он принадлежит выражению более высокого уровня.
*/
double term(double left)
{
    while (true)
    {
        Token t = ts.get();
        switch (t.kind)
        {
            case '*':
                left *= factor();
                break;
            case '/':
            {
                double d = factor();
                if (d == 0)
                    error("divide by zero");
                left /= d;
                break;
            }
            case '%':
            {
                double d = factor();
                if (d == 0)
                    error("modulo by zero");
                left = fmod(left, d);
                break;
            }
            default:
                ts.unget(t); // не наш оператор — отодвинуть токен назад
                return left;
        }
    }
}

// Разбор term «с нуля»: сначала первый factor, потом цикл * / %.
double term() { return term(factor()); }

/*
    factor — степень (упр. 9: оператор «^»), связывает primary оператором ^.
    Приоритет выше, чем у * / %, и ниже, чем у скобок и унарных знаков.
    Правоассоциативен: 2^3^2 разбирается как 2^(3^2) — правая часть
    берётся рекурсией в factor().
*/
double factor()
{
    double left = primary();
    Token t = ts.get();
    if (t.kind == '^')
        return pow(
            left, factor()); // правоассоциативность: правая часть — тоже factor
    ts.unget(t);             // не «^» — токен принадлежит выражению выше
    return left;
}

/*
    expression — сумма/разность, связывает term операторами + и -.
    left — уже разобранная левая часть (первый term либо значение имени).
    То же самое, что и в term(): «чужой» токен отодвигается назад.
*/
double expression(double left)
{
    while (true)
    {
        Token t = ts.get();
        switch (t.kind)
        {
            case '+':
                left += term();
                break;
            case '-':
                left -= term();
                break;
            default:
                ts.unget(t);
                return left;
        }
    }
}

// Разбор expression «с нуля»: сначала term, потом цикл + -.
double expression() { return expression(term()); }

/*
    declaration — разбор объявления вида:  let имя = expression
    Вызывается из statement(), когда первый токен — let.
    Возвращает значение новой величины (оно будет напечатано).
    Повторное объявление проверяет symtab.define().
*/
double declaration()
{
    Token t = ts.get();
    if (t.kind != name)
        error("name expected in declaration");
    string name = t.name;
    Token t2 = ts.get();
    if (t2.kind != '=')
        error("= missing in declaration of ", name);
    double d = expression();
    symtab.define(name, d);
    return d;
}

/*
    statement — одна инструкция (упр. 2: добавлено присваивание):
        - let-объявление:       let имя = expression ;
        - присваивание:         имя = expression ;  (имя уже объявлено,
          иначе symtab.set() бросит «set: undefined name»);
        - иначе:                expression.
    Первый токен читается заранее, чтобы посмотреть, какая это инструкция;
    при присваивании читается ещё и следующий (на «=»).
    «Чужой» токен отодвигается назад (unget), чтобы разобрал expression().
*/
double statement()
{
    Token t = ts.get();
    switch (t.kind)
    {
        case let:
            return declaration();
        case name:
        {
            string s = t.name;
            t = ts.get();
            if (t.kind == '=')
            {
                t = ts.get();
                if (t.kind == print)
                    error("; after = in assignment"); // «x = ;» — правая часть
                                                      // пуста
                ts.unget(t);
                double d = expression();
                symtab.set(s, d);
                return d;
            }
            // Не присваивание: инструкция — выражение, начинающееся с имени s.
            // Имя уже «съедено» — его значение берём из таблицы и разбираем
            // остаток выражения, начиная с готовой левой части (unget'ить
            // два токена нельзя: в Token_stream один буфер).
            ts.unget(t);
            return expression(term(symtab.get(s)));
        }
        default:
            ts.unget(t);
            return expression();
    }
}

/*
    clean_up_mess — «убрать за собой» после ошибки:
    проглотить все символы до следующего ';' ИЛИ до конца строки
    (упр. 5: перевод строки тоже завершает инструкцию), чтобы следующая
    инструкция начиналась «с чистого листа».
*/
void clean_up_mess() { ts.ignore(print); }

/*
    print_help — напечатать справку по команде «help» (упр. 6):
    описание инструкций и команд. Печатается без префикса «= ».
*/
void print_help()
{
    cout << "let x = expr — объявить переменную x" << endl;
    cout << "x = expr     — присвоить значение уже объявленной x" << endl;
    cout << "expr         — выражение (печать его значения)" << endl;
    cout << "Операции: + - * / % (остаток) и ^ (степень, правоассоциативна)"
         << endl;
    cout << "Скобки: ( expr )" << endl;
    cout << "Константы: pi и e (присваиванию не подлежат)" << endl;
    cout << "Конец инструкции: «;» или новая строка (Enter)" << endl;
    cout << "Команды: help — показать эту справку; quit — выход" << endl;
    cout << "(оба не различают регистр)" << endl;
}

/*
    calculate — главный цикл калькулятора:
    напечатать приглашение, прочитать инструкцию, вывести результат.
    Токены «;» (и «\n» после упр. 5) между инструкциями пропускаются.
    После каждой ошибки (catch) очистить ввод до конца строки.
*/
void calculate()
{
    while (true)
    {
        try
        {
            cout << prompt;
            Token t = ts.get();
            while (t.kind == print)
                t = ts.get();
            if (t.kind == quit)
                return;
            // упр. 6: «help» — напечатать справку и продолжить сессию
            if (t.kind == help)
            {
                print_help();
                continue;
            }
            ts.unget(t);
            cout << result << statement() << endl;
        }
        catch (std::runtime_error& e)
        {
            cerr << e.what() << endl;
            clean_up_mess();
        }
    }
}

/*
    main — точка входа.
    Ловит исключения: ошибки из calculate() обрабатываются там,
    а здесь — всё, что просочилось (и сам clean_up_mess не помог).
*/
int main()
try
{
    // Предопределённые константы (как в книге, упр. 3: константы не меняются):
    // pi и e доступны с начала сессии и не подлежат присваиванию.
    symtab.define("pi", 3.14159265358979323, true);
    symtab.define("e", 2.7182818284590452354, true);
    calculate();
    return 0;
}

catch (exception& e)
{
    cerr << "exception: " << e.what() << endl;
    char c;
    while (cin >> c && c != ';')
        ;
    return 1;
}

catch (...)
{
    cerr << "exception\n";
    char c;
    while (cin >> c && c != ';')
        ;
    return 2;
}
