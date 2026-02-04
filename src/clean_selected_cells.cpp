// src/clean_selected_cells.cpp

#include "mainwindow.h"

void MainWindow::cleanSelectedCells () {
    QList<QTableWidgetItem*> selectedItems = dataTable->selectedItems ();
    if (selectedItems.isEmpty ()) return;

    // ПОЛНАЯ БЛОКИРОВКА сигналов
    dataTable->blockSignals (true);

    qDebug () << "--- Запущена ручная очистка ячеек ---";

    for (QTableWidgetItem *item : selectedItems) {
        QString oldText = item->text ();
        if (oldText.isEmpty ()) continue;

        QString text = oldText;

        // Этап "Лечения": Заменяем буквы-двойники на цифры
        text.replace ("З", "3"); // Кириллица
        text.replace ("О", "0"); // Кириллица
        text.replace ("O", "0"); // Латиница
        text.replace ("б", "6");
        text.replace ("ч", "4");
        text.replace ("S", "5");
        text.replace ("s", "5");
        text.replace ("I", "1");
        text.replace ("l", "1");
        text.replace ("|", "1");
        text.replace ("B", "8");
        text.replace (",", "."); // Запятую в точку сразу

        // Удаляем всё, кроме цифр и ОДНОЙ точки
        text.remove (QRegularExpression ("[^0-9.]"));

        if (text.count ('.') > 1) {
            int firstDot = text.indexOf ('.');
            QString part1 = text.left (firstDot + 1);
            QString part2 = text.mid (firstDot + 1).remove ('.');
            text = part1 + part2;
        }

        // Чистим края
        text = text.trimmed ();
        if (text.startsWith (".")) text.remove (0, 1);
        if (text.endsWith (".")) text.chop (1);

        // Если текст изменился — обновляем и логируем
        if (oldText != text) {
            item->setText (text);
            // Принудительно сбрасываем красный фон (ошибка исправлена)
            item->setData (Qt::BackgroundRole, QVariant ());
            qDebug () << QString ("Row %1, Col %2: [%3] -> [%4]")
                        .arg (item->row ()).arg (item->column ()).arg (oldText).arg (text);
        }
    }

    // РАЗБЛОКИРОВКА и принудительный пересчет
    dataTable->blockSignals (false);
    
    // Вызываем расчет один раз вручную, чтобы обновить заголовок окна
    calculateSum ();
}
