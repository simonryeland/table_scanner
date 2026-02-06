// src/settings.cpp

#include "mainwindow.h"
#include <QSettings>
#include <QDir>

void MainWindow::saveSettings () {
    // Параметры: Организация и Название программы
    QSettings settings ("ryeland", "TableScanner");

    settings.beginGroup ("MainWindow");
    settings.setValue ("size", size ());
    settings.setValue ("pos", pos ());
    settings.setValue ("lastFolder", outputFolderPath);
    settings.setValue ("dpi", dpi);
    settings.endGroup ();
}

void MainWindow::loadSettings () {
    QSettings settings ("ryeland", "TableScanner");
    settings.beginGroup ("MainWindow");

    // Сначала загружаем размеры и позицию
    resize (settings.value ("size", QSize (w, h)).toSize ());
    move (settings.value ("pos", QPoint (200, 200)).toPoint ());

    // Загружаем данные. Если в конфиге пусто, используем второй аргумент (дефолт)
    outputFolderPath = settings.value ("lastFolder", QDir::homePath ()).toString ();
    dpi = settings.value ("dpi", 300).toInt ();

    settings.endGroup ();

    // Проверка безопасности: если папка из настроек была удалена с диска
    if (!QDir (outputFolderPath).exists ()) {
        outputFolderPath = QDir::homePath ();
    }

    qDebug () << "Настройки загружены. Путь:" << outputFolderPath;
}
