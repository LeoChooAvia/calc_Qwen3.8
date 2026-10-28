#pragma once

#include "Token.h"

/*
    Token_stream — поток токенов: читает cin, выдаёт токены (get),
    умеет вернуть последний токен назад (unget) и проглотить символы
    до заданного символа (ignore).
    full/buffer — одно-элементный буфер «отката» для unget.
*/
class Token_stream
{
    bool full;
    Token buffer;

  public:
    Token_stream() : full(0), buffer(0) {}

    Token get();
    void unget(Token t)
    {
        buffer = t;
        full = true;
    }

    void ignore(char);
};
