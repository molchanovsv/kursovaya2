TARGET = MusicGui
QT += widgets core5compat
SOURCES += src/main.cpp src/gui/MainWindow.cpp \
    src/gui/StudentActions.cpp src/gui/ConcertActions.cpp src/gui/Dialogs.cpp src/gui/Export.cpp \
    src/core/containers/AVLTree.cpp src/core/DataLoader/DataLoader.cpp src/core/models/FIO.cpp src/core/models/StudentEntry.cpp src/core/models/Teacher.cpp
HEADERS += src/gui/MainWindow.h src/gui/StudentActions.h src/gui/ConcertActions.h src/gui/Dialogs.h src/gui/Export.h \
    src/core/containers/AVLTree.h src/core/containers/HashTable.h src/core/DataLoader/DataLoader.h \
    src/core/models/FIO.h src/core/models/StudentEntry.h src/core/models/Teacher.h src/core/models/ConcertEntry.h
FORMS += src/gui/mainwindow.ui
