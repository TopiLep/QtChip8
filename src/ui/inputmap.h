#ifndef INPUTMAP_H
#define INPUTMAP_H

#include <array>
#include <Qt>
#include <QString>
#include <QSettings>

//keep chip-8 input actions first 16 .
enum class Action {
    Chip8_0, Chip8_1, Chip8_2, Chip8_3,
    Chip8_4, Chip8_5, Chip8_6, Chip8_7,
    Chip8_8, Chip8_9, Chip8_A, Chip8_B,
    Chip8_C, Chip8_D, Chip8_E, Chip8_F,

    //app controlls
    Pause,

    Count
};

class InputMap
{
public:
    InputMap();

    ~InputMap();

    void bind(Action a, int qtKey);
    int getBinding(Action a) const;

    Action actionForKey(int qtKey) const;
    QString actionName(Action a);

    bool isModified() const;

    QSettings m_settings = QSettings("io.github.topilep", "Chip8Emulator");

    void save();
    void load();
    void setDefaults();


private:

    std::array<int, (size_t)Action::Count> bindings{};

    static constexpr std::array<int, (size_t)Action::Count> defaultBindings{
        Qt::Key_X,
        Qt::Key_1,
        Qt::Key_2,
        Qt::Key_3,
        Qt::Key_Q,
        Qt::Key_W,
        Qt::Key_E,
        Qt::Key_A,
        Qt::Key_S,
        Qt::Key_D,
        Qt::Key_Z,
        Qt::Key_C,
        Qt::Key_4,
        Qt::Key_R,
        Qt::Key_F,
        Qt::Key_V,
        Qt::Key_Space
    };

};

#endif // INPUTMAP_H
