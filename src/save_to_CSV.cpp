// src/save_to_CSV.cpp

#include "mainwindow.h"
#include <QFileDialog>
#include <QFile>

void MainWindow::saveToCSV () {
    // Запрашиваем имя файла у пользователя
    QString fileName = QFileDialog::getSaveFileName (this,
        "Сохранить таблицу", "", "CSV Files (*.csv);;All Files (*)");

    if (fileName.isEmpty ()) return;

    // Добавляем расширение, если пользователь его не указал
    if (!fileName.endsWith (".csv")) fileName += ".csv";

    QFile file (fileName);
    if (file.open (QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out (&file);
        // Устанавливаем кодировку UTF-8 для корректного отображения кириллицы
        out.setEncoding (QStringConverter::Utf8);

        for (int r = 0; r < dataTable->rowCount (); ++r) {
            QStringList rowData;
            for (int c = 0; c < dataTable->columnCount (); ++c) {
                QTableWidgetItem *item = dataTable->item (r, c);
                // Если ячейка пустая, пишем пустую строку
                rowData << (item ? item->text () : "");
            }
            // Объединяем ячейки через ";" и пишем в файл
            out << rowData.join (";") << "\n";
        }

        file.close ();
        qDebug () << "Файл успешно сохранен:" << fileName;
    } else {
        qCritical () << "Не удалось открыть файл для записи";
    }
}
