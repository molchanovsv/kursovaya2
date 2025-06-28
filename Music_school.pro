TARGET = MusicGui
QT += widgets core5compat
INCLUDEPATH += include
SOURCES += \
    src/main.cpp \
    src/gui/mainwindow.cpp \
    src/structures/AVLTree.cpp \
    src/data/DataLoader.cpp \
    src/structures/FIO.cpp \
    src/structures/Students_entry.cpp \
    src/structures/Teacher.cpp
HEADERS += \
    include/gui/mainwindow.h \
    include/structures/AVLTree.h \
    include/data/DataLoader.h \
    include/structures/FIO.h \
    include/structures/HashTable.h \
    include/structures/Students_entry.h \
    include/structures/Teacher.h \
    include/structures/Concerts_entry.h
FORMS += mainwindow.ui
