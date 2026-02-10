// src/save_to_CSV.cpp

#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>

void MainWindow::saveToCSV () {
    // Используем последнюю удачную папку из настроек
    QString initialPath = outputFolderPath + "/result.csv";

    QString fileName = QFileDialog::getSaveFileName(this,
        "Сохранить таблицу", initialPath, "CSV Files (*.csv)");

    if (fileName.isEmpty ()) return;

    // ФИЛЬТРАЦИЯ: Проверяем путь на кириллицу перед сохранением
    if (containsCyrillic (fileName)) {
        QMessageBox::StandardButton res = QMessageBox::warning(this, "Опасный путь", 
            "В пути или имени файла обнаружена кириллица. Это может вызвать проблемы в Excel.\n"
            "Продолжить всё равно?", QMessageBox::Yes | QMessageBox::No);
        if (res == QMessageBox::No) return;
    }

    if (!fileName.endsWith (".csv")) fileName += ".csv";

    QFile file (fileName);
    if (file.open (QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out (&file);
        
        // BOM (Byte Order Mark) — КРИТИЧНО для Excel
        // Без этого Excel может открыть файл "кракозябрами", даже если там UTF-8
        out.setGenerateByteOrderMark (true);
        out.setEncoding (QStringConverter::Utf8);

        // Записываем разделитель специально для Excel (чтобы он сразу разбил по столбцам)
        out << "sep=;\n"; 

        for (int r = 0; r < dataTable->rowCount (); ++r) {
            QStringList rowData;
            for (int c = 0; c < dataTable->columnCount (); ++c) {
                QTableWidgetItem *item = dataTable->item (r, c);
                QString text = item ? item->text () : "";
                
                // Экранируем кавычки и точки с запятой, если они есть в тексте
                if (text.contains (";") || text.contains ("\"")) {
                    text = "\"" + text.replace ("\"", "\"\"") + "\"";
                }
                rowData << text;
            }
            out << rowData.join (";") << "\n";
        }

        file.close ();
        
        // Обновляем путь для следующего раза
        outputFolderPath = QFileInfo (fileName).absolutePath ();
        saveSettings ();
        
        qDebug () << "Файл сохранен (BOM UTF-8):" << fileName;
    }
}
