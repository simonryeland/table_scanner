// src/check_system_dependencies.cpp

#include "mainwindow.h"
#include <QStringList>
#include <QMessageBox>
#include <QProcess>

void MainWindow::checkSystemDependencies () {
    QStringList missingPackages;

    // Список пакетов для проверки (runtime зависимости)
    QStringList packages = {
        "tesseract-ocr-ukr",
        "tesseract-ocr-rus",
        "tesseract-ocr-eng",
        "sane-utils"
    };

    for (const QString &package : packages) {
        QProcess check;
        // dpkg-query -W возвращает 0, если пакет установлен
        check.start ("dpkg-query", {"-W", "-f='${Status}'", package});
        check.waitForFinished ();

        if (check.exitCode () != 0) {
            missingPackages << package;
        }
    }

    // Если чего-то не хватает — выводим сообщение с готовой командой
    if (!missingPackages.isEmpty ()) {
        QString command = "sudo apt update && sudo apt install " + missingPackages.join (" ");

        QMessageBox messageBox (this);
        messageBox.setIcon (QMessageBox::Warning);
        messageBox.setWindowTitle ("Отсутствуют компоненты");
        messageBox.setText ("Для корректной работы распознавания необходимо установить дополнительные пакеты.");
        messageBox.setInformativeText ("Скопируйте и выполните команду в терминале:\n\n" + command);
        messageBox.setDetailedText ("Список отсутствующих пакетов:\n" + missingPackages.join ("\n"));
        messageBox.exec ();
    }
}
