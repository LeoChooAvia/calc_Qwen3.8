/*
    Token_stream — поток токенов (глава 7).
    Читает символы из cin и собирает из них токены:
    одиночные символы-операторы, числа, имена и ключевые слова.
    Умеет возвращать прочитанный токен назад (unget) — «откат» на один токен.
*/

#include "Token_stream.h"
#include "SymbolicConstants.h"
#include <cctype>    // isalpha, isdigit, isspace
#include <stdexcept> // runtime_error

/*
    get — следующий токен из cin.
    Если буфер занят (ранее был unget), сначала вернуть его содержимое.
    Символы читаются через cin.get() (упр. 5): пропускаются все
    пробельные символы (isspace), кроме перевода строки «\n»;
    «\n» — это отдельный токен Token(print): конец инструкции.
    Числа и имена собираются посимвольно, «вперёд откатывается»
    только первый символ (cin.unget).
    EOF — Token(quit): сессия завершается.
*/
Token Token_stream::get()
{
    if (full)
    {
        full = false;
        return buffer;
    }
    char ch;
    do
    {
        // cin.get не «пропускает» символы сам: EOF бросаем как Token(quit),
        // иначе ch не обновится и get() зациклится.
        if (!cin.get(ch))
            return Token(quit);
    } while (isspace(static_cast<unsigned char>(ch)) && ch != '\n');
    if (ch == '\n')
        return Token(print); // перевод строки — конец инструкции (упр. 5)
    switch (ch)
    {
        case '(':
        case ')':
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '^':
        case ';':
        case '=':
            return Token(ch);
        case '.':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            cin.unget(); // вернуть символ в cin...
            double val;
            if (!(cin >> val))
            {
                // число не распознано (например, «...»): сбрасываем failbit,
                // иначе следующая проверка EOF в get() ошибочно завершит
                // сессию.
                cin.clear();
                error("number expected");
            }
            return Token(number, val);
        }
        default:
            if (isalpha(ch))
            {
                string s;
                s += ch;
                // имя — буквы, цифры и «_» (упр. 1: «_» разрешено внутри имени)
                while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_'))
                    s += ch; // приписывать каждый символ к имени
                cin.unget();
                if (s == "let")
                    return Token(let); // «let» — только строчное (как в книге)
                // упр. 6+7: «quit» и «help» не различают регистр (QQuUiItT и
                // т.п.)
                string lower = s;
                for (char& c : lower)
                    c = tolower(static_cast<unsigned char>(c));
                if (lower == "quit")
                    return Token(quit); // «quit» — это выход, а не имя
                if (lower == "help")
                    return Token(help); // «help» — показать справку, а не имя
                return Token(name, s);  // имя переменной — с исходным регистром
            }
            error("Bad token");
    }
}

/*
    ignore — проглатывать символы из cin, пока не встретится символ c
    ИЛИ перевод строки «\n» (упр. 5): после ошибки «дочитываем» остаток
    строки, чтобы следующая инструкция читалась с чистого листа.
    Символы читаем через cin.get(): с оператором «>>» пробельные
    символы (включая «\n») были бы проглочены, и «остановка» на «\n»
    не сработала бы.
    Если c уже в буфере (unget), просто сбрасываем буфер.
*/
void Token_stream::ignore(char c)
{
    if (full && c == buffer.kind)
    {
        full = false;
        return;
    }
    full = false;

    char ch;
    while (cin.get(ch))
        if (ch == c || ch == '\n')
            return;
}
