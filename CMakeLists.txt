cmake_minimum_required(VERSION 3.16)
project(MusicSchool)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 COMPONENTS Widgets Core5Compat REQUIRED)

set(GUI_SOURCES
    src/gui/mainwindow.cpp
    src/gui/student_actions.cpp
    src/gui/concert_actions.cpp
    src/gui/dialogs.cpp
    src/gui/export.cpp
    src/gui/theme.cpp
)

set(CORE_SOURCES
    src/core/containers/avltree.cpp
    src/core/containers/hashtable.cpp
    src/core/dataloader/dataloader.cpp
    src/core/validators.cpp
)

set(HEADERS
    src/gui/mainwindow.h
    src/core/containers/avltree.h
    src/core/containers/hashtable.h
    src/core/dataloader/dataloader.h
    src/core/models/fio.h
    src/core/models/student_entry.h
    src/core/models/teacher.h
    src/core/models/concert_entry.h
    src/core/validators.h
)

qt5_wrap_ui(UI_SOURCES src/gui/mainwindow.ui)
qt5_add_resources(ICON_RESOURCES resources/icons.qrc)

add_executable(MusicGui src/main.cpp ${GUI_SOURCES} ${CORE_SOURCES} ${UI_SOURCES} ${ICON_RESOURCES})

target_include_directories(MusicGui PRIVATE
    src
    src/core
    src/core/models
    src/core/containers
    src/core/dataloader
    src/gui
)

target_link_libraries(MusicGui Qt5::Widgets Qt5::Core5Compat)

