// src/acquire_from_scanner.cpp

#include "mainwindow.h"
#include <QFile>
#include <QProcess>
#include <QMessageBox>

void MainWindow::acquireFromScanner () {
    cleanTempImages ();

    // Определяем путь для временного сохранения скана
//    QString tempPath = QDir::tempPath () + "/scanner_output.jpg";
    QString tempPath = "tmp/scanner_output.jpg";

    // Удаляем старый файл, если он остался от предыдущего раза
    if (QFile::exists (tempPath)) {
        QFile::remove (tempPath);
    }

    // Настраиваем процесс вызова SANE
    QProcess *scanProcess = new QProcess (this);

    progressBar->setRange (0, 0);
    progressBar->setFormat ("Идет сканирование...");
    recognizerButton->setEnabled (false); // Блокируем кнопки

    recognizer.setDpi (this->dpi);

    {
        // Аргументы для scanimage:
        // --format=jpeg : сохраняем сразу в сжатый формат
        // --resolution 300 : стандарт для качественного OCR
        // --mode Color : цветное сканирование

        QStringList scanArgs;
        scanArgs << "--device-name=plustek:libusb:003:002"
                  << "--format=png"
                  << "--mode" << "Gray"
                  << "--resolution" << QString::number (dpi)
                  << "-x" << "210"
                  << "-y" << "150"
                  << "--output-file" << tempPath;

        //QProcess *magickProcess = new QProcess (this);
        //scanProcess->setStandardOutputProcess (magickProcess);

        //QStringList scanArgs;
        //scanArgs << "--device-name=plustek:libusb:003:002"
                  //<< "--mode" << "Gray"
                  //<< "-x" << "210" << "-y" << "150"
                  //<< "--resolution" << QString::number (dpi/2);
        //QStringList magickArgs;
        //magickArgs << "-" << "-negate" << "-normalize" << "-negate" << "-level" << "60%,80%,2.0" << tempPath;

        // Визуальный фидбек пользователю
        this->setWindowTitle ("Идет сканирование... Ждите.");
        recognizerButton->setEnabled (false); // Блокируем кнопку, чтобы не нажимали дважды

        // Запускаем процесс
        scanProcess->start ("scanimage", scanArgs);
        //magickProcess->start ("magick", magickArgs);
        scanProcess->waitForFinished ();
        //magickProcess->waitForFinished ();
    }

    // до 60 секунд для медленных сканеров)
   if (scanProcess->waitForFinished (60000) && QFile::exists (tempPath)) {

        // Переключаем прогресс в режим завершения
        progressBar->setRange (0, 100);
        progressBar->setValue (100);
        progressBar->setFormat ("Сканирование завершено.");

        // Выполняем OCR
        processImageFile (tempPath);

        progressBar->setFormat ("Готово");
    } else {
        progressBar->setRange (0, 100);
        progressBar->setValue (0);
        progressBar->setFormat ("Ошибка сканера");
        QMessageBox::critical (this, "Ошибка", "Не удалось получить изображение.");
    }

    // Проверяем результат и запускаем обработку
    if (QFile::exists (tempPath)) {
        this->setWindowTitle ("Скан получен. Начинаю распознавание...");

        // ВЫЗОВ ОБЩЕЙ ЛОГИКИ
        processImageFile (tempPath);

        this->setWindowTitle ("Готово");
    } else {
        QString errorLog = scanProcess->readAllStandardError ();
        QMessageBox::warning (this, "Ошибка", "Файл не создан. Лог: " + errorLog);
    }

    recognizerButton->setEnabled (true);
}
