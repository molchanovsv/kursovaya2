TARGET = MusicGui
QT += widgets core5compat
SOURCES += src/main.cpp src/gui/mainwindow.cpp \
    src/gui/student_actions.cpp src/gui/concert_actions.cpp src/gui/dialogs.cpp src/gui/export.cpp \
    src/core/containers/avltree.cpp src/core/containers/hashtable.cpp src/core/dataloader/dataloader.cpp
HEADERS += src/gui/mainwindow.h \
    src/core/containers/avltree.h src/core/containers/hashtable.h src/core/dataloader/dataloader.h \
    src/core/models/fio.h src/core/models/student_entry.h src/core/models/teacher.h src/core/models/concert_entry.h
FORMS += src/gui/mainwindow.ui

# Include directories for headers
INCLUDEPATH += \
    src \
    src/core \
    src/core/models \
    src/core/containers \
    src/core/dataloader \
    src/gui
