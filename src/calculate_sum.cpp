// src/calculate_sum.cpp

#include "mainwindow.h"

void MainWindow::calculateSum () {
    static int lastProcessedCol = -1;
    if (dataTable->rowCount () == 0) return;

    int targetColumn = dataTable->currentColumn ();
    if (targetColumn < 0) return;

    bool isManualEdit = (sender () == dataTable);
    if (targetColumn == lastProcessedCol && !isManualEdit) return;

    if (isManualEdit) lastProcessedCol = -1;
    else lastProcessedCol = targetColumn;

    dataTable->blockSignals (true);

    // Сброс цветов (только для НЕ выделенных)
    for (int r = 0; r < dataTable->rowCount (); ++r) {
        for (int c = 0; c < dataTable->columnCount (); ++c) {
            if (auto *item = dataTable->item (r, c)) {
                // Если ячейка выделена - не трогаем её вообще!
                if (!item->isSelected ()) {
                    item->setData (Qt::BackgroundRole, QVariant ());
                }
            }
        }
    }

    double total = 0;
    for (int i = 0; i < dataTable->rowCount (); ++i) {
        if (QTableWidgetItem *item = dataTable->item(i, targetColumn)) {
            QString rawText = item->text().trimmed();
            if (rawText.isEmpty()) {
                if (!item->isSelected()) item->setData(Qt::BackgroundRole, QVariant());
                continue;
            }

            // ОДИН РАЗ парсим значение
            bool ok;
            // Мы используем логику cleanAndParse, но нам нужно знать, "ок" ли число
            // Поэтому лучше вынести проверку прямо сюда или доработать cleanAndParse
            double value = cleanAndParse(rawText);

            // Проверяем, валидно ли число (чистое ли оно)
            // Если cleanAndParse вернул число, а исходный текст содержал мусор (кроме точек/запятых)
            // то ok будет false. Но проще всего проверить через toDouble.
            rawText.replace(",", ".");
            rawText.toDouble(&ok);

            if (ok) {
                total += value;

                // Если была запятая или лишние точки — принудительно обновляем текст в ячейке
                if (item->text().contains(",") || item->text().count('.') > 1) {
                    item->setText(QString::number(value, 'f', 2));
                }

                if (!item->isSelected()) {
                    item->setData(Qt::BackgroundRole, QVariant());
                }
            } else {
                // Ошибка (если в строке есть буквы или это не число)
                item->setBackground(QColor("#5c2121"));
            }
        }
    }

    // Обновляем заголовки
    for (int i = 0; i < dataTable->columnCount (); ++i) {
        if (auto *header = dataTable->horizontalHeaderItem (i)) {
            if (i == targetColumn) header->setBackground (QColor ("#235377"));
            else header->setData (Qt::BackgroundRole, QVariant ());
        }
    }

    total = std::round (total * 100.0) / 100.0;
    QString columnLetter = QString (QChar ('A' + targetColumn));
    this->setWindowTitle (QString ("Сумма столбца %1: %2").arg (columnLetter).arg (total, 0, 'f', 2));

    dataTable->blockSignals (false);
}

