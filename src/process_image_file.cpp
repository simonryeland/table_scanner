// src/process_image_file.cpp

#include "mainwindow.h"
#include "processor.h"
#include <QCoreApplication> // Для qApp
#include <QStatusBar>
#include <QHeaderView>

// Универсальный метод обработки (вызывается из диалога или из сканера)
void MainWindow::processImageFile (const QString &path) {
    TableProcessor processor;

    // Загружаем изображение через OpenCV (наш класс Processor)
    if (!processor.loadImage (path)) {
        qDebug () << "Не удалось загрузить изображение:" << path;
        return;
    }

    // Инициализируем распознаватель
    TableRecognizer recognizer;
    // Устанавливаем режим (цифры или текст) из состояния чекбокса
    recognizer.setNumericMode (numericOnlyCheck->isChecked ());

    // Извлекаем ячейки (нарезка OpenCV)
    QVector<QVector<QImage>> tableData = processor.extractCells ();

    if (tableData.isEmpty ()) {
        qDebug () << "Таблица не найдена на изображении.";
        return;
    }

    // Очистка
    dataTable->setUpdatesEnabled (false);
    dataTable->clear ();
    dataTable->setRowCount (0);
    dataTable->setColumnCount (0);

    // tableData — это QVector<QVector<QImage>>, где внешний вектор — это СТРОКИ
    int totalRows = tableData.size ();

    for (int row = 0; row < totalRows; ++row) {
        bool hasData = false;
        QStringList rowTexts; // Используем список для накопления текста строки

        int colsInThisRow = tableData[row].size ();

        // Сначала только распознаем ВСЕ ячейки в текущей строке
        for (int col = 0; col < colsInThisRow; ++col) {
            QString text = recognizer.recognizeCell (tableData[row][col]);
            if (!text.isEmpty ()) {
                hasData = true;
            }
            rowTexts.append (text);
        }

        // Только ЕСЛИ в строке есть данные — добавляем ОДНУ строку в таблицу
        if (hasData) {
            int newRowIndex = dataTable->rowCount ();
            dataTable->insertRow (newRowIndex);

            // Обновляем количество колонок, если в этой строке их больше
            if (rowTexts.size () > dataTable->columnCount ()) {
                dataTable->setColumnCount (rowTexts.size ());
            }

            // Заполняем добавленную строку текстом
            for (int col = 0; col < rowTexts.size (); ++col) {
                QTableWidgetItem *item = new QTableWidgetItem (rowTexts[col]);

                // Разрешаем редактирование и взаимодействие
                item->setFlags (item->flags () | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                // Центрируем текст для красоты
                item->setTextAlignment (Qt::AlignCenter);

                dataTable->setItem (newRowIndex, col, item);
            }
        }

        // Обновление прогресса
        int progress = ((row + 1) * 100) / totalRows;
        progressBar->setValue (progress);
        qApp->processEvents ();
    }

    progressBar->setValue (100);
    progressBar->setFormat ("Готово!");
    statusBar ()->showMessage (QString ("Распознано строк: %1 | Столбцов: %2")
                         .arg (dataTable->rowCount ())
                         .arg (dataTable->columnCount ()));

    // Заставляет Qt немедленно обработать все события отрисовки
    qApp->processEvents (QEventLoop::ExcludeUserInputEvents);

        dataTable->setUpdatesEnabled (true);
        dataTable->horizontalHeader ()->setSectionResizeMode (QHeaderView::Interactive);
        dataTable->resizeColumnsToContents ();

        // DEBUG!!!
        qDebug() << "Обработка файла завершена:" << path;
    }
