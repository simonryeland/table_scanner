// include/mainwindow.h

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>
#include <QCheckBox>
#include <QProgressBar>
#include <QCloseEvent> 
#include <QString>
#include "types.h"
#include "recognizer.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow (QWidget *parent = nullptr);
    ~MainWindow ();

private slots:
    void calculateSum ();           // математические действия
    void selectOutputFolder ();     // для выбора папки
    void saveToCSV ();              // для сохранения
    void handleStartProcessing ();  // Запуск OCR и OpenCV
    void acquireFromScanner ();     // Для коннекта к сканеру
    void deleteCellAndShiftLeft ();
    void showHelp ();

protected:
    void closeEvent (QCloseEvent *event) override;

private:
    int w = 1111;
    int h = 500;
    int dpi = 300;

    TableRecognizer recognizer;
    QString outputFolderPath;

    void setupUI ();                             // для ручной сборки интерфейса
    void processImageFile (const QString &path); // Вспомогательный метод
    void cleanTempImages ();                     // для очищения tmp/
    void cleanSelectedCells ();
    void checkSystemDependencies ();
    void loadSettings ();
    void saveSettings ();

    // Утилиты для работы с текстом
    double cleanAndParse (QString text);
    QString sanitizeFilename (QString name);
    bool containsCyrillic (const QString &str);

    // Указатели на виджеты
    QWidget *centralWidget;
    QTableWidget *dataTable;
    QPushButton *helpButton;
    QPushButton *scanButton;
    QPushButton *recognizerButton;
    QPushButton *calculateButton;
    QPushButton *saveButton;
    QPushButton *quitButton;
    QPushButton *selectFolderButton;
    QCheckBox *numericOnlyCheck;
    QProgressBar *progressBar;
};

#endif // MAINWINDOW_H
