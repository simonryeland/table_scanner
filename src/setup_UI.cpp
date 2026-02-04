// src/setupUI.cpp

#include "mainwindow.h"
#include <QCoreApplication> // Для qApp
#include <QShortcut>
#include <QHeaderView>
#include <QStatusBar>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QHBoxLayout>

void MainWindow::setupUI () {
    centralWidget = new QWidget (this);
    setCentralWidget (centralWidget);
    auto *mainLayout = new QVBoxLayout (centralWidget);

    numericOnlyCheck = new QCheckBox ("Только цифры", this);

    // Кнопки управления
    auto *buttonLayout = new QHBoxLayout ();
    helpButton = new QPushButton ("Help", this);
    helpButton->setFixedWidth (74);
    helpButton->setIcon (QIcon (":/icons/help.png"));
    helpButton->setIconSize (QSize (16, 16));
    helpButton->setObjectName ("helpButton");
    helpButton->setStyleSheet (
        "QPushButton#helpButton:hover {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #626200, stop:1 #4b4b00);"
        "    color: white;"
        "    border: 1px solid #a3a300;"
        "}"
        "QPushButton#helpButton:pressed {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4b4b00, stop:1 #323200);"
        "    border: 1px solid #646400;"
        "}"
    );
    scanButton = new QPushButton ("Получить со сканера ...", this);
    recognizerButton = new QPushButton ("Распознать из файла ...", this);
    selectFolderButton = new QPushButton ("Выбрать папку для сохранения", this);
    calculateButton = new QPushButton ("Посчитать сумму", this);
    saveButton = new QPushButton ("Сохранить в Excel/CSV", this);

    // button Exit
    quitButton = new QPushButton ("Exit", this);
    quitButton->setFixedWidth (70);
    quitButton->setToolTip (tr ("Выход из программы (Ctrl+Q или Esc)"));
    QAction *quitAction = new QAction (this);
    quitAction->setShortcuts ({QKeySequence (Qt::CTRL | Qt::Key_Q), QKeySequence (Qt::Key_Escape)});
    connect (quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    connect (quitButton, &QPushButton::clicked, quitAction, &QAction::trigger);
    this->addAction (quitAction);
    quitButton->setIcon (QIcon (":/icons/exit.png"));
    quitButton->setIconSize (QSize (16, 16));
    quitButton->setObjectName ("quitButton");
    quitButton->setStyleSheet (
        "QPushButton#quitButton:hover {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #e74c3c, stop:1 #942b21);"
        "    color: white;"
        "    border: 1px solid #ff8787;"
        "}"
        "QPushButton#quitButton:pressed {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #c0392b, stop:1 #43130f);"
        "    border: 1px solid #ff4939;"
        "}"
    );

    // Таблица
    dataTable = new QTableWidget (12, 7, this); // 12 строк, 7 колонок
    dataTable->horizontalHeader ()->setSectionResizeMode (QHeaderView::Stretch);
    // Создаем "горячую клавишу" Delete для таблицы
    QShortcut *deleteShortcut = new QShortcut (QKeySequence (Qt::Key_Delete), dataTable);
    deleteShortcut->setContext (Qt::WidgetShortcut);

    dataTable->setSelectionMode (QAbstractItemView::ExtendedSelection); // Разрешает Ctrl и Shift (Разрешаем множественный выбор)
    dataTable->setSelectionBehavior (QAbstractItemView::SelectItems);   // Выделяем именно ячейки, а не строки
    // Отключаем автоматическое выделение ячеек при клике на заголовок
    // Запрещаем заголовкам выделять колонки
    dataTable->horizontalHeader()->setHighlightSections(false);
    // Важно: установите это, чтобы клик не сбрасывал выделение при расчетах
    dataTable->setFocusPolicy(Qt::StrongFocus);
    // Запрещаем заголовкам забирать фокус
    dataTable->horizontalHeader()->setFocusPolicy(Qt::NoFocus);
    // флаг, который иногда заставляет фокус прыгать
    dataTable->horizontalHeader ()->setSectionsClickable (true);
    // Это заставит ячейку "просыпаться" при первом же клике
    dataTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);

    // ПРОВЕРКА НАЛИЧИЯ SCANIMAGE
    QString scanImagePath = QStandardPaths::findExecutable ("scanimage");

    if (scanImagePath.isEmpty ()) {
        // Утилита не найдена
        scanButton->setEnabled (false);
        scanButton->setText ("Сканер недоступен (установите sane-utils)");
        scanButton->setToolTip ("В системе не найдена утилита scanimage. Выполните: sudo apt install sane-utils");
    } else {
        // Все хорошо, подключаем слот
        connect (scanButton, &QPushButton::clicked, this, &MainWindow::acquireFromScanner);
    }

    // Создаем статус-бар внизу окна
    setStatusBar (new QStatusBar (this));
    statusBar ()->showMessage ("Готов к работе");

    // Создаем индикатор
    progressBar = new QProgressBar (this);
    progressBar->setTextVisible (true);
    progressBar->setFormat ("Ожидание...");
    progressBar->setValue (0);

    buttonLayout->addWidget (helpButton);
    buttonLayout->addWidget (scanButton);
    buttonLayout->addWidget (recognizerButton);
    buttonLayout->addWidget (calculateButton);

    buttonLayout->addWidget (numericOnlyCheck);

    buttonLayout->addWidget (selectFolderButton);
    buttonLayout->addWidget (saveButton);
    buttonLayout->addWidget (quitButton);

    mainLayout->addLayout (buttonLayout);
    mainLayout->addWidget (dataTable);

    mainLayout->addWidget (progressBar);

    // Создаем шорткат для удаления со сдвигом
    QShortcut *shiftDelShortcut = new QShortcut (QKeySequence ("Shift+Delete"), dataTable);
    shiftDelShortcut->setContext (Qt::WidgetShortcut);

    // Создаем шорткат для клавиши `
    QShortcut *cleanShortcut = new QShortcut (QKeySequence (Qt::Key_QuoteLeft), dataTable);
    cleanShortcut->setContext (Qt::WidgetShortcut);

    // Коннекты
    connect (helpButton, &QPushButton::clicked, this, &MainWindow::showHelp);
    connect (recognizerButton, &QPushButton::clicked, this, &MainWindow::handleStartProcessing);
    connect (calculateButton, &QPushButton::clicked, this, &MainWindow::calculateSum);
    connect (selectFolderButton, &QPushButton::clicked, this, &MainWindow::selectOutputFolder);
    connect (saveButton, &QPushButton::clicked, this, &MainWindow::saveToCSV);
    // Пересчитывать при клике на ячейку

    // Подключаем к лямбда-функции для удаления
    connect (deleteShortcut, &QShortcut::activated, this, [this]() {
        QList<QTableWidgetItem*> selectedItems = dataTable->selectedItems();
        if (selectedItems.isEmpty ()) return;

        // Собираем уникальные индексы строк
        QSet<int> rowsToDelete;
        for (auto *item : selectedItems) {
            rowsToDelete.insert(item->row ());
        }

        // Преобразуем в список и сортируем от большего к меньшему
        QList<int> sortedRows = rowsToDelete.values ();
        std::sort (sortedRows.begin (), sortedRows.end (), std::greater<int>());

        // Удаляем
        dataTable->blockSignals (true);
        for (int row : sortedRows) {
            dataTable->removeRow (row);
        }
        dataTable->blockSignals (false);

        calculateSum ();
    });

    connect (shiftDelShortcut, &QShortcut::activated, this, &MainWindow::deleteCellAndShiftLeft);

    connect (cleanShortcut, &QShortcut::activated, this, &MainWindow::cleanSelectedCells);

    // Пересчет при ручной правке текста пользователем
    connect (dataTable, &QTableWidget::itemChanged, this, &MainWindow::calculateSum);
    // ЕДИНСТВЕННЫЙ коннект для кликов и Ctrl-выделения
    // Мы используем QueuedConnection, чтобы дать Qt завершить прорисовку синего фона
    connect (dataTable, &QTableWidget::itemSelectionChanged, this, [this]() {
        QMetaObject::invokeMethod(this, "calculateSum", Qt::QueuedConnection);
    });
    // Пересчет при клике на заголовок (без смены ячейки)
    connect (dataTable->horizontalHeader (), &QHeaderView::sectionClicked, this, [this](int){
        calculateSum ();
    });

    connect (dataTable, &QTableWidget::itemSelectionChanged, this, [this]() {
            calculateSum ();
            qDebug () << "*** Выделение обновлено ***";
        });

    connect (dataTable, &QTableWidget::itemClicked, this, [this](QTableWidgetItem *item) {
        if (item) {
            qDebug () << "- Клик! Координаты: [" << item->row () << "," << item->column () << "]";
            qDebug () << "- Текст:" << item->text ();

            // Получаем все выделенные ячейки (если их несколько)
            QList<QTableWidgetItem*> selectedItems = dataTable->selectedItems ();
            qDebug () << "- Всего выделено ячеек:" << selectedItems.size ();
        }
    });

    connect (dataTable, &QTableWidget::cellDoubleClicked, this, [this](int r, int c){
        qDebug () << "Проверка ячейки:" << r << c;
    });
}
