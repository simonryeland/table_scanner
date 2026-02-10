// src/main.cpp

#include "mainwindow.h"
#include <QApplication>
#include <QFile>

void loadStyle (QApplication &app) {
    QFile file (":/style.qss");
    if (file.open (QFile::ReadOnly)) {
        QString styleSheet = QLatin1String (file.readAll ());
        app.setStyleSheet (styleSheet);
    }
}

int main (int argc, char **argv)
{
    QApplication a (argc, argv);
    loadStyle (a); // Стили применятся ко всем окнам
    MainWindow w;
    w.show ();
    return a.exec ();
}
