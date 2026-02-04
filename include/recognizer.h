// include/recognizer.h

#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <tesseract/baseapi.h>
#include <QImage>
#include <QString>

class TableRecognizer {
public:
    TableRecognizer ();
    ~TableRecognizer ();

    // Распознает текст из одной картинки-ячейки
    QString recognizeCell (const QImage &cellImg);
    void setNumericMode (bool enabled);
    void setDpi (int value);

private:
    tesseract::TessBaseAPI *tessApi;
    bool m_isNumericMode;
    int m_dpi = 300;
};

#endif // RECOGNIZER_H
