// src/processor.cpp

#include "processor.h"
#include <QDebug>
#include <algorithm>
#include <vector>
#include <cmath>

TableProcessor::TableProcessor () {}

bool TableProcessor::loadImage (const QString &path) {
    sourceImage = cv::imread (path.toStdString ());
    return !sourceImage.empty ();
}

QImage TableProcessor::matToQImage (const cv::Mat &mat) {
    if (mat.empty ()) return QImage ();

    if (mat.channels () == 1) {
        // Если изображение черно-белое (после фильтров)
        return QImage (mat.data, mat.cols, mat.rows,
                      static_cast<int>(mat.step),
                      QImage::Format_Grayscale8).copy ();
    }

    // Если изображение цветное (оригинал)
    return QImage (mat.data, mat.cols, mat.rows,
                  static_cast<int>(mat.step),
                  QImage::Format_BGR888).copy ();
}

QVector<QVector<QImage>> TableProcessor::extractCells () {
    if (sourceImage.empty ()) return {};

    // Предварительная подготовка оригинала
    // ПЕРВЫМ ДЕЛОМ ВЫРАВНИВАЕМ
    deskewImage (sourceImage);

    cv::Mat gray;

    //// Повышение резкости
    //cv::Mat blurred;
    //cv::GaussianBlur (gray, blurred, cv::Size (0, 0), 3);
    //cv::addWeighted (gray, 1.5, blurred, -0.5, 0, gray);

    //// Удаление шума
    //cv::fastNlMeansDenoising (gray, gray, 10);

    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE (2.0, cv::Size (8,8));
    clahe->apply (gray, gray);

    // Создание processedForOcr

    // Меняем блок повышения резкости и Denoising на этот:
    cv::Mat processedForOcr;
    cv::cvtColor (sourceImage, gray, cv::COLOR_BGR2GRAY);

    // Мягкое удаление мелкого мусора (соль с перцем), не портящее края букв
    cv::GaussianBlur (gray, gray, cv::Size (5,5), 3);

    // Адаптивная бинаризация (увеличим размер блока до 15 для стабильности было (11, 2))
    cv::medianBlur (gray, gray, 3);
    cv::adaptiveThreshold (gray, processedForOcr, 255,
                          cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                          cv::THRESH_BINARY, 15, 5); // 5 — это константа вычитания, убирает серый шум

    // Поиск линий (используем свежесозданный processedForOcr)  (инвертируем маску, так как морфология ищет белое на черном)
    cv::Mat linesMask = ~processedForOcr;  // Инверсия (белое становится черным и наоборот)
    cv::Mat horizontal = linesMask.clone ();
    cv::Mat vertical = linesMask.clone ();

    // Горизонтальные линии
    cv::Mat hElem = cv::getStructuringElement (cv::MORPH_RECT, cv::Size (horizontal.cols / 10, 1));
    cv::morphologyEx(horizontal, horizontal, cv::MORPH_OPEN, hElem);

    // Вертикальные линии
    cv::Mat vElem = cv::getStructuringElement (cv::MORPH_RECT, cv::Size (1, vertical.rows / 10));
    cv::morphologyEx (vertical, vertical, cv::MORPH_OPEN, vElem);

    cv::Mat tableMask = horizontal + vertical;
    cv::morphologyEx (tableMask, tableMask, cv::MORPH_CLOSE, cv::getStructuringElement (cv::MORPH_RECT, cv::Size (3,3)));

    // Сделать линии жирнее, чтобы объединить разрывы
    cv::Mat kernel = cv::getStructuringElement (cv::MORPH_RECT, cv::Size (3, 3));
    cv::dilate (tableMask, tableMask, kernel);

    // Поиск контуров
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours (tableMask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Rect> rects;
    for (const auto& cnt : contours) {
        cv::Rect r = cv::boundingRect (cnt);
        // Игнорируем всё, что меньше определенного размера
        // (на скане 300dpi ячейка обычно шире 80 пикселей)
        if (r.width < 60 || r.height < 25) continue;

        // ЗАЩИТА ОТ ВЛОЖЕННОСТИ:
        // Если уже добавили большой прямоугольник, который содержит этот,
        // или этот прямоугольник почти дублирует существующий — игнорируем.
        bool overlap = false;
        for (const auto& existing : rects) {
            // Проверяем, находится ли центр текущего прямоугольника внутри уже добавленного
            cv::Point center (r.x + r.width/2, r.y + r.height/2);
            if (existing.contains (center)) {
                overlap = true;
                break;
            }
        }
        if (!overlap) rects.push_back (r);
    }

    if (rects.empty ()) return {}; // Проверка перед созданием currentRow

    // Группировка строк
    std::sort (rects.begin (), rects.end (), [](const cv::Rect& a, const cv::Rect& b) {
        return a.y < b.y;
    });

    QVector<QVector<cv::Rect>> rows;
    QVector<cv::Rect> currentRow;
    currentRow.push_back (rects[0]);

    for (size_t i = 1; i < rects.size (); ++i) {
        cv::Rect r = rects[i];
        cv::Rect last = currentRow.back ();
        int y_top = std::max (r.y, last.y);
        int y_bottom = std::min (r.y + r.height, last.y + last.height);
        int overlap = y_bottom - y_top;
        int minHeight = std::min (r.height, last.height);

        if (overlap > minHeight * 0.4) {
            currentRow.push_back (r);
        } else {
            std::sort (currentRow.begin (), currentRow.end (), [](const cv::Rect& a, const cv::Rect& b) {
                return a.x < b.x;
            });
            rows.append (currentRow);

            // Начинаем новую строку
            currentRow.clear ();
            currentRow.push_back (r);
        }
    }

    // Добавляем последнюю накопленную строку
    if (!currentRow.isEmpty ()) {
        std::sort (currentRow.begin (), currentRow.end(), [](const cv::Rect& a, const cv::Rect& b) {
            return a.x < b.x;
        });
        rows.append (currentRow);
    }

    // DEBUG!!!
    qDebug () << "Найдено контуров после фильтрации:" << rects.size ();
    cv::imwrite (QString ("tmp/last_full_mask.png").toStdString (), tableMask);

    // Нарезка ячеек (с расчетом пропусков и фильтрами для OCR)
    int avgWidth = 0;
    for (auto& r : rects) avgWidth += r.width;
    avgWidth /= rects.size ();

    QVector<QVector<QImage>> finalTable;
    int rowIdx = 0;
    for (const auto& rowRects : rows) {
        QVector<QImage> images;
        int expectedX = -1;
        int colIdx = 0;
        for (const auto& r : rowRects) {
            // Пропуск ячейки
            if (expectedX != -1 && (r.x - expectedX) > avgWidth * 0.8) {
                images.append (QImage ());
            }

            // Создаем уменьшенный прямоугольник (отступ 5 пикселей)
            int offset = 5;
            // Делаем область чуть меньше (отступ offset пикселя внутрь от рамки)
            cv::Rect innerRect (r.x + offset, r.y + offset, r.width - (offset * 2), r.height - (offset * 2));

            // Проверка, чтобы не выйти за границы картинки
            innerRect &= cv::Rect (0, 0, processedForOcr.cols, processedForOcr.rows);

            // Вырезаем из обработанного (бинарного) изображения
            cv::Mat cell = processedForOcr (innerRect).clone ();

            // Добавляем микро-утолщение букв (Erosion в OpenCV для ЧБ маски делает черное жирнее)
//            cv::Mat element = cv::getStructuringElement (cv::MORPH_RECT, cv::Size (2, 2));
//            cv::erode (cell, cell, element); // Буквы станут четче

            // ФИЛЬТРЫ ДЛЯ ТОЧНОСТИ (обязательно !!!) Увеличиваем ячейку в 2 раза
            cv::resize (cell, cell, cv::Size (), 3.0, 3.0, cv::INTER_CUBIC);

            // легкое размытие, чтобы убрать острые углы пикселей
            //cv::GaussianBlur (cell, cell, cv::Size (3, 3), 0);

            // Это сделает цифры тоньше, а пространство рядом с точкой — чище
            cv::Mat kernel = cv::getStructuringElement (cv::MORPH_RECT, cv::Size (2, 2));
            cv::erode (cell, cell, kernel);

            // Добавляем 15 пикселей белого поля со всех сторон
            int border = 15;
            cv::copyMakeBorder (cell, cell, border, border, border, border, cv::BORDER_CONSTANT, cv::Scalar (255));

            // DEBUG!!!
            QString debugPath = QString ("tmp/cell_%1_%2.png").arg (rowIdx).arg (colIdx);
            cv::imwrite (debugPath.toStdString (), cell);

            images.append (matToQImage (cell));
            expectedX = r.x + r.width;
            colIdx++;
        }
        finalTable.append (images);
        rowIdx++;
    }
    return finalTable;
}

void TableProcessor::deskewImage (cv::Mat &img) {
    cv::Mat gray, thresh;
    if (img.channels () == 3) cv::cvtColor (img, gray, cv::COLOR_BGR2GRAY);
    else gray = img;

    // Бинаризация для поиска линий
    cv::threshold (gray, thresh, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

    // Находим все точки, которые не являются фоном
    std::vector<cv::Point> points;
    cv::findNonZero (thresh, points);

    // Вычисляем минимальный прямоугольник, описывающий все точки
    cv::RotatedRect box = cv::minAreaRect (points);

    double angle = box.angle;
    // Корректировка угла OpenCV (в зависимости от версии)
    if (angle < -45.0) angle += 90.0;
    else if (box.size.width < box.size.height) angle += 90.0;

    // Если наклон минимальный (меньше 0.5 градуса), ничего не делаем
    if (std::abs (angle) < 0.5 || std::abs (angle) > 20.0) return;

    // Матрица поворота
    cv::Point2f center = box.center;
    cv::Mat M = cv::getRotationMatrix2D (center, angle, 1.0);

    // Выполняем поворот
    cv::warpAffine (img, img, M, img.size (), cv::INTER_CUBIC, cv::BORDER_REPLICATE);

    qDebug () << "Изображение выровнено. Угол:" << angle;
}
