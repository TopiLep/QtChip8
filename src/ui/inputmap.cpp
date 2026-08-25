#include "inputmap.h"



InputMap::InputMap()
{
    load();
}

InputMap::~InputMap()
{
    save();
}

void InputMap::bind(Action a, int qtKey)
{
    bindings[(size_t)a] = qtKey;
}

int InputMap::getBinding(Action a) const
{
    return bindings[(size_t)a];
}

Action InputMap::actionForKey(int qtKey) const
{
    for (size_t i = 0; i < bindings.size(); ++i)
        if (bindings[i] == qtKey) return (Action)i;
    return Action::Count;
}

QString InputMap::actionName(Action a)
{
    switch (a) {
    case Action::Chip8_0: return "0";
    case Action::Chip8_1: return "1";
    case Action::Chip8_2: return "2";
    case Action::Chip8_3: return "3";
    case Action::Chip8_4: return "4";
    case Action::Chip8_5: return "5";
    case Action::Chip8_6: return "6";
    case Action::Chip8_7: return "7";
    case Action::Chip8_8: return "8";
    case Action::Chip8_9: return "9";
    case Action::Chip8_A: return "A";
    case Action::Chip8_B: return "B";
    case Action::Chip8_C: return "C";
    case Action::Chip8_D: return "D";
    case Action::Chip8_E: return "E";
    case Action::Chip8_F: return "F";
    case Action::Pause: return "Pause";
    default: return "Unknown";
    }
}

bool InputMap::isModified() const
{
    return bindings != defaultBindings;
}

void InputMap::save()
{
    m_settings.beginWriteArray("keybinds");
    for (int i = 0; i < (int)Action::Count; i++) {
        m_settings.setArrayIndex(i);
        m_settings.setValue("key", bindings[i]);
    }
    m_settings.endArray();
}

void InputMap::load()
{
    int size = m_settings.beginReadArray("keybinds");
    if (size == (int)Action::Count) {
        for (int i = 0; i < size; ++i) {
            m_settings.setArrayIndex(i);
            bindings[i] = m_settings.value("key").toInt();
        }
    } else {
        setDefaults();
    }
    m_settings.endArray();
}



void InputMap::setDefaults()
{
    bindings = defaultBindings;
}


