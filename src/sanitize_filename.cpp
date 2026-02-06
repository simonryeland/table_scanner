// src/sanitize_filename.cpp

#include "mainwindow.h"

QString MainWindow::sanitizeFilename (QString name) {
    // Заменяем кириллицу на латиницу (транслитерация) или просто удаляем
    // Здесь самый простой путь — заменить все "неправильное" на "_"
    // Регулярное выражение [^a-zA-Z0-9._-] означает: 
    // "всё, что НЕ буква латиницы, НЕ цифра, НЕ точка, НЕ тире и НЕ подчеркивание"
    name.replace(QRegularExpression ("[^a-zA-Z0-9._-]"), "_");

    // Убираем двойные подчеркивания, если они возникли
    while (name.contains ("__")) name.replace ("__", "_");

    return name;
}
