#include "inputmap.h"



InputMap::InputMap()
{
    setDefaults();
}

void InputMap::bind(Action a, int qtKey)
{
    bindings[(size_t)a] = qtKey;
}

Action InputMap::actionForKey(int qtKey) const
{
    for (size_t i = 0; i < bindings.size(); ++i)
        if (bindings[i] == qtKey) return (Action)i;
    return Action::Count;
}

void InputMap::setDefaults()
{
    bindings[(size_t)Action::Chip8_0] = Qt::Key_X;
    bindings[(size_t)Action::Chip8_1] = Qt::Key_1;
    bindings[(size_t)Action::Chip8_2] = Qt::Key_2;
    bindings[(size_t)Action::Chip8_3] = Qt::Key_3;
    bindings[(size_t)Action::Chip8_4] = Qt::Key_Q;
    bindings[(size_t)Action::Chip8_5] = Qt::Key_W;
    bindings[(size_t)Action::Chip8_6] = Qt::Key_E;
    bindings[(size_t)Action::Chip8_7] = Qt::Key_A;
    bindings[(size_t)Action::Chip8_8] = Qt::Key_S;
    bindings[(size_t)Action::Chip8_9] = Qt::Key_D;
    bindings[(size_t)Action::Chip8_A] = Qt::Key_Z;
    bindings[(size_t)Action::Chip8_B] = Qt::Key_C;
    bindings[(size_t)Action::Chip8_C] = Qt::Key_4;
    bindings[(size_t)Action::Chip8_D] = Qt::Key_R;
    bindings[(size_t)Action::Chip8_E] = Qt::Key_F;
    bindings[(size_t)Action::Chip8_F] = Qt::Key_V;

    bindings[(size_t)Action::Pause] = Qt::Key_Space;
}
