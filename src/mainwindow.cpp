// src/mainwindow.cpp

#include "mainwindow.h"
#include <QTimer>

MainWindow::MainWindow (QWidget *parent) : QMainWindow (parent) {
    setupUI ();

    QTimer::singleShot (0, this, [this]() {
        loadSettings ();
    });

    QTimer::singleShot (500, this, [this]() {
        checkSystemDependencies ();
    });
}

MainWindow::~MainWindow () {}
