#ifndef INPUTMAP_H
#define INPUTMAP_H

#include <array>
#include <Qt>

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

    void bind(Action a, int qtKey);
    Action actionForKey(int qtKey) const;

private:
    void setDefaults();
    std::array<int, (size_t)Action::Count> bindings{};

};

#endif // INPUTMAP_H
