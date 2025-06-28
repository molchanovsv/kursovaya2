TARGET = MusicGui
QT += widgets core5compat
SOURCES += main.cpp mainwindow.cpp \
    StudentActions.cpp ConcertActions.cpp Dialogs.cpp Export.cpp \
    AVLTree.cpp DataLoader.cpp FIO.cpp Students_entry.cpp Teacher.cpp
HEADERS += mainwindow.h StudentActions.h ConcertActions.h Dialogs.h Export.h \
    AVLTree.h DataLoader.h FIO.h HashTable.h Students_entry.h Teacher.h Concerts_entry.h
FORMS += mainwindow.ui
