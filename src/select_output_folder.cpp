// src/select_output_folder.cpp

#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>

void MainWindow::selectOutputFolder () {
    QString dir = QFileDialog::getExistingDirectory (this, "Выберите папку", outputFolderPath);
    
    if (!dir.isEmpty ()) {
        // Проверка на наличие не-ASCII символов (кириллицы)
        bool hasNonAscii = false;
        for (const QChar &c : dir) {
            if (c.unicode () > 127) {
                hasNonAscii = true;
                break;
            }
        }

        if (hasNonAscii) {
            QMessageBox::warning (this, "Внимание", 
                "Путь содержит кириллицу. Это может привести к ошибкам при сохранении файлов.\n"
                "Рекомендуется использовать пути только на латинице.");
        }
        
        outputFolderPath = dir;
        saveSettings (); // Сразу сохраняем "плохой" или "хороший" путь
    }
}
