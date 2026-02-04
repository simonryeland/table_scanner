// src/close_event.cpp

#include "mainwindow.h"
#include <QCloseEvent>

void MainWindow::closeEvent (QCloseEvent *event)
{
//    loadSettings ();
    event->accept ();
}
