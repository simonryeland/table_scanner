// src/check_system_dependencies.cpp

#include "mainwindow.h"
#include <QStringList>
#include <QMessageBox>
#include <QProcess>
#include <QGuiApplication>
#include <QClipboard>
#include <QStandardPaths>

void MainWindow::checkSystemDependencies () {
    QStringList missing;

    // Список необходимых системных пакетов
    QStringList packages = {
        "sane-utils",
        "libsane1",
        "opencv-data",
        "tesseract-ocr-ukr",
        "tesseract-ocr-rus",
        "tesseract-ocr-eng"
    };

    for (const QString &package : packages) {
        QProcess check;
        // Используем dpkg-query для проверки статуса установки (только для Debian/Ubuntu)
        check.start ("dpkg-query", {"-W", "-f='${Status}'", package});
        check.waitForFinished();

        QString output = check.readAllStandardOutput ();
        if (check.exitCode () != 0 || !output.contains ("installed")) {
            missing << package;
        }
    }

    // Дополнительная проверка: доступен ли исполняемый файл сканера
    if (QStandardPaths::findExecutable ("scanimage").isEmpty ()) {
        if (!missing.contains ("sane-utils")) missing << "sane-utils";
    }

    // Вывод предупреждения
    if (!missing.isEmpty ()) {
        QString installCmd = "sudo apt update && sudo apt install " + missing.join (" ");

        QMessageBox msgBox (this);
        msgBox.setIcon (QMessageBox::Warning);
        msgBox.setWindowTitle ("Проверка компонентов");
        msgBox.setText ("В системе отсутствуют компоненты, необходимые для работы:");
        msgBox.setInformativeText (missing.join (", ") +
            "\n\nДля исправления выполните в терминале:\n" + installCmd);

        // Добавим кнопку "Копировать команду" для удобства пользователя
        QPushButton *copyBtn = msgBox.addButton ("Копировать команду", QMessageBox::ActionRole);
        msgBox.addButton (QMessageBox::Ok);

        msgBox.exec ();

        if (msgBox.clickedButton () == copyBtn) {
            QGuiApplication::clipboard ()->setText (installCmd);
        }
    }
}
