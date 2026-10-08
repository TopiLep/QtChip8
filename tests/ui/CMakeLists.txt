qt_add_executable(chip8_preferences_tests
    preferences_tests.cpp
    ../../src/ui/preferenceswindow.cpp
    ../../src/ui/preferenceswindow.h
    ../../src/ui/preferenceswindow.ui
    ../../src/ui/inputmap.cpp
    ../../src/ui/inputmap.h
    ../../src/ui/keybindbutton.cpp
    ../../src/ui/keybindbutton.h
    ../../src/ui/colorpickerbutton.cpp
    ../../src/ui/colorpickerbutton.h
)
target_include_directories(chip8_preferences_tests PRIVATE ../../src/ui)
target_link_libraries(chip8_preferences_tests PRIVATE chip8 Qt::Widgets)
add_test(NAME chip8_preferences COMMAND chip8_preferences_tests)
set_tests_properties(chip8_preferences PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
