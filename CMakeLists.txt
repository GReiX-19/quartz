cmake_minimum_required(VERSION 3.21)
project(Quartz VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml QuickControls2 Multimedia)

qt_standard_project_setup()

qt_add_library(backend STATIC)
qt_add_qml_module(backend
    URI ${PROJECT_NAME}.Backend
    VERSION 1.0
    SOURCES
        src/backend/Player.h
        src/backend/Player.cpp
        src/backend/PlaylistModel.h
        src/backend/PlaylistModel.cpp
)
target_include_directories(backend PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/backend
)
target_link_libraries(backend PUBLIC
    Qt6::Core
    Qt6::Qml
    Qt6::Multimedia
)

qt_add_executable(quartz
    src/main.cpp
)
qt_add_qml_module(quartz
    URI ${PROJECT_NAME}
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/PlaylistPanel.qml
)
target_link_libraries(quartz PRIVATE
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    backend
    backendplugin
)

include(GNUInstallDirs)
install(TARGETS quartz
    DESTINATION ${CMAKE_INSTALL_BINDIR})