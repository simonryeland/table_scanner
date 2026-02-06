TARGET = tabscan
TEMPLATE = app

QT += core gui widgets

CONFIG += c++17 link_pkgconfig

PKGCONFIG += tesseract opencv4

LIBS += -lsane

INCLUDEPATH += include/

HEADERS += include/mainwindow.h \
           include/processor.h \
           include/recognizer.h \
           include/scanner.h

SOURCES += src/main.cpp \
           src/mainwindow.cpp \
           src/setup_UI.cpp \
           src/processor.cpp \
           src/recognizer.cpp \
           src/calculate_sum.cpp \
           src/clean_and_parse.cpp \
           src/check_system_dependencies.cpp \
           src/acquire_from_scanner.cpp \
           src/handle_start_processing.cpp \
           src/process_image_file.cpp \
           src/select_output_folder.cpp \
           src/save_to_CSV.cpp \
           src/clean_temp_images.cpp \
           src/delete_cell_and_shift_left.cpp \
           src/clean_selected_cells.cpp \
           src/close_event.cpp \
           src/help.cpp \
           src/settings.cpp \
           src/sanitize_filename.cpp \
           src/contains_cyrillic.cpp

RESOURCES += rc/resources.qrc

MOC_DIR = build_tmp/moc
OBJECTS_DIR = build_tmp/obj
DESTDIR = .
