#include "keybindbutton.h"


KeybindButton::KeybindButton(int currentKey, QWidget *parent)
    : QPushButton(parent), m_key(currentKey)
{
    refreshText();
}

void KeybindButton::setKey(int qtKey)
{
    m_key = qtKey;
    refreshText();
}

void KeybindButton::mousePressEvent(QMouseEvent *e)
{
    m_capturing = true;
    setText("Press a key...");
    setFocus();
    grabKeyboard();
    QPushButton::mousePressEvent(e);
}

void KeybindButton::keyPressEvent(QKeyEvent *e)
{
    if (!m_capturing) {
        QPushButton::keyPressEvent(e);
        return;
    }

    m_key = e->key();
    m_capturing = false;
    releaseKeyboard();
    refreshText();
    emit keyCaptured(m_key);
}

void KeybindButton::focusOutEvent(QFocusEvent *e)
{
    if (m_capturing) {
        m_capturing = false;
        releaseKeyboard();
        refreshText();
    }
    QPushButton::focusOutEvent(e);
}

void KeybindButton::refreshText()
{
    setText(QKeySequence(m_key).toString());
}

