// src/clean_temp_images.cpp

#include "mainwindow.h"
#include <QDir>

void MainWindow::cleanTempImages () {
    QDir tempDir ("tmp");
    QStringList filters;
    filters << "cell_*.png";

    QStringList tempFiles = tempDir.entryList (filters, QDir::Files);

    for (const QString &fileName : tempFiles) {
        tempDir.remove (fileName);
    }

    qDebug () << "Очищено временных файлов:" << tempFiles.size ();
}
