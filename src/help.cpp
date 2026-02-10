// src/help.cpp

#include "mainwindow.h"
#include <QMessageBox>

void MainWindow::showHelp ()
{
    QMessageBox helpBox (this);
    helpBox.setWindowTitle ("Справка: Table OCR Scanner");

    // Используем HTML для оформления
    QString helpText =
        "<h3><font color='#3d5afe'>Горячие клавиши:</font></h3>"
        "<ul>"
        "<li><b>Delete</b> — удалить выбранные строки целиком.</li>"
        "<li><b>Shift + Delete</b> — удалить ячейку и сместить строку влево.</li>"
        "<li><b>Enter / Клик</b> — ручное редактирование текста.</li>"
        "</ul>"
        "<h3><font color='#3d5afe'>Инструкция:</font></h3>"
        "<p>1. Нажмите <b>Сканировать</b> или загрузите файл.<br>"
        "2. Нажмите на <b>заголовок столбца</b> (A, B, C...), чтобы рассчитать сумму.<br>"
        "3. Ячейки с ошибками подсвечиваются <font color='#5c2121'>красным</font>.</p>"
        "<i>Программа автоматически прокручивает таблицу к первой ошибке.</i>";

    helpBox.setTextFormat (Qt::RichText);
    helpBox.setText (helpText);
    helpBox.setStandardButtons (QMessageBox::Ok);

    helpBox.exec();
}
