// src/contains_cyrillic.cpp

#include "mainwindow.h"

bool MainWindow::containsCyrillic (const QString &str) {
    for (const QChar &c : str) {
        if (c.unicode () > 127) return true;
    }
    return false;
}
