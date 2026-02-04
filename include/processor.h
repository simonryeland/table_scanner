// include/processor.h

#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <opencv2/opencv.hpp>
#include <QImage>
#include <QString>
#include <QVector>

class TableProcessor {
public:
    TableProcessor ();
    bool loadImage (const QString &path);
    // Возвращает список вырезанных ячеек как QImage для Tesseract
    QVector<QVector<QImage>> extractCells (); 

private:
    cv::Mat sourceImage;
    cv::Mat processedImage;
    QImage matToQImage (const cv::Mat &mat);
    void deskewImage (cv::Mat &img); // Внутренний метод для выравнивания
};

#endif //PROCESSOR_H
