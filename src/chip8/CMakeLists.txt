add_library(chip8
    chip8.cpp
    chip8.h
)

target_include_directories(chip8 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
