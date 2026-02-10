// include/scanner.h

#ifndef SCANNER_H
#define SCANNER_H

#include <sane/sane.h>
#include <QString>

class DocumentScanner {
public:
    DocumentScanner ();
    ~DocumentScanner ();
    // Возвращает путь к отсканированному изображению
    QString scanDocument (); 

private:
    SANE_Handle handle;
};

#endif // SCANNER_H
