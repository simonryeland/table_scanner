// src/recognizer.cpp

#include "recognizer.h"
#include <QDebug>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QDir>

TableRecognizer::TableRecognizer () {
    tessApi = new tesseract::TessBaseAPI ();

    // Внутри AppImage путь будет: bin/../share/tessdata
    QString appDir = QCoreApplication::applicationDirPath ();
    QString tessPath = appDir + "/../share/tessdata";

    // Если запускаем просто через терминал (разработка), проверяем системный путь
    if (!QDir (tessPath).exists ()) {
        tessPath = "/usr/share/tesseract-ocr/5/tessdata";
    }

    if (tessApi->Init (tessPath.toUtf8 ().data (), "ukr+rus+eng")) {
        qCritical () << "Tesseract Init Failed! Path:" << tessPath;
    }

    // Инициализация (убедитесь, что стоят пакеты tesseract-ocr-ukr/rus/eng)
    if (tessApi->Init (NULL, "ukr+rus+eng")) {
        qCritical () << "Could not initialize Tesseract OCR";
    }
    m_isNumericMode = false; // Инициализируем по умолчанию
    // Режим PSM_SINGLE_LINE идеален для ячеек таблицы
    tessApi->SetPageSegMode (tesseract::PSM_SINGLE_LINE);

    if (m_isNumericMode) {
        // Добавляем запятую в белый список, иначе Tesseract её проигнорирует
        tessApi->SetVariable ("tessedit_char_whitelist", "0123456789.,-");
    }

    //if (m_isNumericMode) {
        //// Для цифр: СТРОГО отключаем словари и включаем Whitelist
        //tessApi->SetVariable ("load_system_dawg", "0");
        //tessApi->SetVariable ("load_freq_dawg", "0");
        //tessApi->SetVariable ("tessedit_char_whitelist", "0123456789.");
    //} else {
        //// Для текста: ВКЛЮЧАЕМ словари для помощи в распознавании слов
        //tessApi->SetVariable ("load_system_dawg", "1");
        //tessApi->SetVariable ("load_freq_dawg", "1");
        //tessApi->SetVariable ("tessedit_char_whitelist", ""); // Убираем ограничения
    //}

    // Отключаем поиск слов в словаре, чтобы он читал символы как они есть
    tessApi->SetVariable ("load_system_dawg", "0");
    tessApi->SetVariable ("load_freq_dawg", "0");
    tessApi->SetVariable ("textord_heavy_nr", "1"); // Помогает при наличии шума
    tessApi->SetVariable ("load_punc_dawg", "0");
    tessApi->SetVariable ("load_number_dawg", "0");
    tessApi->SetVariable ("load_unambig_dawg", "0");
    tessApi->SetVariable ("load_bigram_dawg", "0");
    tessApi->SetVariable ("load_fixed_length_dawg", "0");
}

TableRecognizer::~TableRecognizer () {
    tessApi->End ();
    delete tessApi;
}

void TableRecognizer::setDpi (int value) {
    m_dpi = value;
}

QString TableRecognizer::recognizeCell (const QImage &cellImg) {
    if (cellImg.isNull ()) return QString ();

    // Подготовка изображения
    QImage img = cellImg.convertToFormat (QImage::Format_RGB888);

    // Передача изображения и установка DPI
    tessApi->SetImage (img.bits (), img.width (), img.height (), 3, img.bytesPerLine ());
    tessApi->SetSourceResolution (m_dpi);

    // Настройка режима сегментации (PSM_SINGLE_WORD - для ячеек идеален)
    tessApi->SetPageSegMode (tesseract::PSM_SINGLE_LINE);

    // Установка ограничений (Whitelist)
    if (m_isNumericMode) {
        tessApi->SetVariable ("tessedit_char_whitelist", "0123456789.,-");
    } else {
        // Добавляем украинские буквы (і, ї, є, ґ) в белый список
        tessApi->SetVariable ("tessedit_char_whitelist",
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
            "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя"
            "ІЇЄҐіїєґ.,' ");
    }

    // Получение текста
    char *outText = tessApi->GetUTF8Text ();
    if (!outText) return "";

    QString result = QString::fromUtf8 (outText).trimmed ();
    delete[] outText;

    if (m_isNumericMode) {
        // Предварительная замена частых ошибок
        result.replace (" ", ""); // Убираем случайные пробелы (например, "375 . 13")
        result.replace (",", "."); // Сразу к точке
        result.replace ("З", "3");
        result.replace ("О", "0");
        result.replace ("O", "0");

        // Очистка (оставляем только цифры и точку)
        result.remove (QRegularExpression ("[^0-9.]"));

        // Если точек несколько (например "375..13"), оставляем одну
        while (result.contains ("..")) result.replace ("..", ".");

        // Убираем точки по краям (мусор)
        if (result.startsWith (".")) result.remove (0, 1);
        if (result.endsWith (".")) result.chop (1);

        if (result.isEmpty () || result == ".") return "";
    } else {
        // ТЕКСТОВЫЙ РЕЖИМ: очистка от одиночного шума
        // Если длина < 2 и нет букв/цифр — это шум от рамки (типа "_" или "|")
        if (result.length () < 2 && !result.contains (QRegularExpression ("[0-9A-Za-zА-Яа-яІЇЄҐіїєґ]"))) {
            return "";
        }
    }

    return result.trimmed ();
}

void TableRecognizer::setNumericMode (bool enabled) {
    if (enabled) {
        // Устанавливаем белый список символов
        tessApi->SetVariable ("tessedit_char_whitelist", "0123456789.,-");
    } else {
        // Сбрасываем ограничения (пустая строка или nullptr могут не сработать во всех версиях)
        // Лучше просто инициализировать заново или оставить пустым
        tessApi->SetVariable ("tessedit_char_whitelist", "");
    }
    m_isNumericMode = enabled; // Сохраняем состояние
}
