// src/select_output_folder.cpp

#include "mainwindow.h"
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>

void MainWindow::selectOutputFolder () {
    // Используем статический метод для выбора директории
    outputFolderPath = QFileDialog::getExistingDirectory (this,
        "Выберите папку для сохранения результатов",
        QDir::homePath (),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!outputFolderPath.isEmpty ()) {
        qDebug () << "Выбрана папка:" << outputFolderPath;
        selectFolderButton->setText ("Папка: " + QFileInfo(outputFolderPath).baseName ());
    }
}
