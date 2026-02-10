// src/close_event.cpp

#include "mainwindow.h"

void MainWindow::closeEvent (QCloseEvent *event) {
    saveSettings ();
    event->accept ();
}
