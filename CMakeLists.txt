cmake_minimum_required(VERSION 3.19)
project(Chip8Emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Multimedia)

qt_standard_project_setup()

add_subdirectory(src/chip8)


qt_add_executable(Chip8Emulator
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/ui/mainwindow.ui
    src/ui/displaywidget.h src/ui/displaywidget.cpp
    src/ui/inputmap.h src/ui/inputmap.cpp
    src/ui/memorywindow.h src/ui/memorywindow.cpp src/ui/memorywindow.ui
)

qt_add_resources(Chip8Emulator "sounds"
    PREFIX "/"
    FILES
        sounds/beep.wav
)

target_include_directories(Chip8Emulator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

target_link_libraries(Chip8Emulator
    PRIVATE
        chip8
        Qt::Core
        Qt::Widgets
        Qt::Multimedia
)

install(TARGETS Chip8Emulator
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Chip8Emulator
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
