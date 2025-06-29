cmake_minimum_required(VERSION 3.16)
project(MusicSchool)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 COMPONENTS Widgets Core5Compat REQUIRED)

set(GUI_SOURCES
    src/gui/MainWindow.cpp
    src/gui/StudentActions.cpp
    src/gui/ConcertActions.cpp
    src/gui/Dialogs.cpp
    src/gui/Export.cpp
)

set(CORE_SOURCES
    src/core/containers/AVLTree.cpp
    src/core/containers/HashTable.cpp
    src/core/DataLoader/DataLoader.cpp
    src/core/models/FIO.cpp
    src/core/models/StudentEntry.cpp
    src/core/models/Teacher.cpp
)

set(HEADERS
    src/gui/MainWindow.h
    src/core/containers/AVLTree.h
    src/core/containers/HashTable.h
    src/core/DataLoader/DataLoader.h
    src/core/models/FIO.h
    src/core/models/StudentEntry.h
    src/core/models/Teacher.h
    src/core/models/ConcertEntry.h
)

qt5_wrap_ui(UI_SOURCES src/gui/mainwindow.ui)

add_executable(MusicGui src/main.cpp ${GUI_SOURCES} ${CORE_SOURCES} ${UI_SOURCES})

target_include_directories(MusicGui PRIVATE
    src
    src/core
    src/core/models
    src/core/containers
    src/core/DataLoader
    src/gui
)

target_link_libraries(MusicGui Qt5::Widgets Qt5::Core5Compat)

