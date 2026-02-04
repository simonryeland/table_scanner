// src/mainwindow.cpp

#include "mainwindow.h"
#include <QTimer>

MainWindow::MainWindow (QWidget *parent) : QMainWindow (parent) {
    setupUI ();
    setWindowTitle ("Scanner Tables");
    setWindowIcon (QIcon (":/icons/app_icon.png"));
    setMinimumSize (800, 240);
    resize (w, h);

    QTimer::singleShot (500, this, [this]() {
        checkSystemDependencies ();
    });

//    QTimer::singleShot (0, this, [=]() {
//        loadSettings (); // Выполнится сразу после того, как конструктор завершит работу
//    });
}

MainWindow::~MainWindow () {}
