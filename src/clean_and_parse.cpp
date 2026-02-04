// src/clean_and_parse.cpp

#include "mainwindow.h"
#include <QString>
#include <QRegularExpression>

double MainWindow::cleanAndParse (QString text) {
    text = text.trimmed ();
    text.replace (",", "."); // Заменяем запятые на точки

    // Удаляем всё, кроме цифр и точек
    text.remove (QRegularExpression ("[^0-9.]"));

    // Если точек больше одной — оставляем только первую, остальные удаляем
    if (text.count ('.') > 1) {
        int firstDot = text.indexOf ('.');
        QString part1 = text.left (firstDot + 1); // Число до первой точки включительно
        QString part2 = text.mid (firstDot + 1).remove ('.'); // Всё остальное без точек
        text = part1 + part2;
    }

    // Убираем точку, если она в самом конце (например, "123.")
    if (text.endsWith ('.')) text.chop (1);

    bool ok;
    double val = text.toDouble (&ok);

    // Если всё же не удалось (например, пустая строка), возвращаем 0
    return ok ? val : 0.0;
}
