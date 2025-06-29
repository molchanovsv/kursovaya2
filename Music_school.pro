TARGET = MusicGui
QT += widgets core5compat
SOURCES += src/main.cpp src/gui/MainWindow.cpp \
    src/gui/StudentActions.cpp src/gui/ConcertActions.cpp src/gui/Dialogs.cpp src/gui/Export.cpp \
    src/core/containers/AVLTree.cpp src/core/containers/HashTable.cpp src/core/DataLoader/DataLoader.cpp src/core/models/FIO.cpp src/core/models/StudentEntry.cpp src/core/models/Teacher.cpp
HEADERS += src/gui/MainWindow.h \
    src/core/containers/AVLTree.h src/core/containers/HashTable.h src/core/DataLoader/DataLoader.h \
    src/core/models/FIO.h src/core/models/StudentEntry.h src/core/models/Teacher.h src/core/models/ConcertEntry.h
FORMS += src/gui/mainwindow.ui

# Include directories for headers
INCLUDEPATH += \
    src \
    src/core \
    src/core/models \
    src/core/containers \
    src/core/DataLoader \
    src/gui
