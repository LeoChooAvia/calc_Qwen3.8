#!/usr/bin/env python3
"""Тесты калькулятора (задание 1, подпункт 4-5).

Схема:
    Запускается build/calculator, в stdin подаётся tests_in.txt.
    stdout и stderr захватываются отдельно и сравниваются с эталоном:
        tests_out_expected.txt  — эталонный stdout
        tests_err_expected.txt  — эталонный stderr
    Файлы actual_out.txt / actual_err.txt пишутся рядом для сравнения
    руками при FAIL.

Формат эталона stdout:
    На каждую инструкцию калькулятор печатает приглашение "> ".
    Если инструкция вычислена успешно, за ним на той же строке
    печатается "= <значение>" (через endl).
    Если инструкция дала ошибку, строка остаётся просто "> "
    (текст ошибки уходит в stderr); если ошибка произошла ещё до
    печати "= ", строка тоже просто "> ".

Успех: код возврата 0, stdout и stderr совпали с эталоном байт в байт.
"""

import os
import subprocess

HERE = os.path.dirname(os.path.abspath(__file__))
BINARY = os.path.join(HERE, "..", "build", "calculator")
TIMEOUT = 10  # секунд; вешание считается падением


def read_bytes(name):
    with open(os.path.join(HERE, name), "rb") as f:
        return f.read()


def write_bytes(name, data):
    with open(os.path.join(HERE, name), "wb") as f:
        f.write(data)


def diff(expected, actual):
    """Построчный diff: строки, отличающиеся от эталона."""
    lines = []
    exp = expected.decode("utf-8", "replace").splitlines()
    act = actual.decode("utf-8", "replace").splitlines()
    for i in range(max(len(exp), len(act))):
        e = exp[i] if i < len(exp) else "<нет строки>"
        a = act[i] if i < len(act) else "<нет строки>"
        if e != a:
            lines.append("строка %d: эталон: %r; факт: %r" % (i + 1, e, a))
    return lines


def main():
    if not os.path.exists(BINARY):
        print("FAIL: бинарник не найден: %s (сначала собрать: cmake --build build)" % BINARY)
        return 1

    data_in = read_bytes("tests_in.txt")
    try:
        proc = subprocess.run(
            [BINARY],
            input=data_in,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=TIMEOUT,
        )
    except subprocess.TimeoutExpired:
        print("FAIL: бинарник не завершился за %d секунд (вешание)" % TIMEOUT)
        return 1

    out = proc.stdout
    err = proc.stderr
    write_bytes("actual_out.txt", out)
    write_bytes("actual_err.txt", err)

    ok = True

    if proc.returncode != 0:
        print("FAIL: код возврата %d (ожидалось 0)" % proc.returncode)
        ok = False
    else:
        print("код возврата: 0")

    expected_out = read_bytes("tests_out_expected.txt")
    if out != expected_out:
        ok = False
        print("FAIL: stdout не совпадает с эталоном:")
        for line in diff(expected_out, out):
            print("    " + line)
    else:
        print("stdout: совпадает с эталоном")

    expected_err = read_bytes("tests_err_expected.txt")
    if err != expected_err:
        ok = False
        print("FAIL: stderr не совпадает с эталоном:")
        for line in diff(expected_err, err):
            print("    " + line)
    else:
        print("stderr: совпадает с эталоном")

    print("PASS" if ok else "FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
