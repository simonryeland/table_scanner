// src/handle_start_processing.cpp

#include "mainwindow.h"
#include <QFileDialog>

// Метод для кнопки "Открыть файл"
void MainWindow::handleStartProcessing () {
    // Открываем диалог выбора файла
    QString fileName = QFileDialog::getOpenFileName (
        this,
        "Выберите изображение таблицы",
        "",
        "Images (*.png *.jpg *.jpeg *.bmp)"
    );

    // Если выбран файл — запускаем обработку
    if (!fileName.isEmpty ()) {
        processImageFile (fileName);
    }
}
