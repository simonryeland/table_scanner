// src/delete_cell_and_shift_left.cpp

#include "mainwindow.h"

void MainWindow::deleteCellAndShiftLeft () {
    // Получаем список индексов из модели выделения
    QModelIndexList selectedIndexes = dataTable->selectionModel ()->selectedIndexes ();

    if (selectedIndexes.isEmpty ()) {
        qDebug () << "Ничего не выбрано!";
        return;
    }

    // Берем индекс последней выбранной ячейки (активной)
    QModelIndex index = selectedIndexes.last ();
    int row = index.row ();
    int col = index.column ();

    qDebug() << "РЕАЛЬНО удаляем в строке:" << row << "колонке:" << col;

    int colCount = dataTable->columnCount ();

    // Блокируем сигналы, чтобы не мешать перемещению данных
    dataTable->blockSignals(true);

    // Сдвиг
    for (int i = col; i < colCount - 1; ++i) {
        QTableWidgetItem *nextItem = dataTable->item (row, i + 1);
        if (nextItem) {
            dataTable->setItem (row, i, new QTableWidgetItem (*nextItem));
        } else {
            delete dataTable->takeItem (row, i);
        }
    }

    // Очистка хвоста строки
    delete dataTable->takeItem (row, colCount - 1);

    dataTable->blockSignals(false);

    // Пересчет
    calculateSum ();
}
