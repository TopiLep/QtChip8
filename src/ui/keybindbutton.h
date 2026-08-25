#ifndef KEYBINDBUTTON_H
#define KEYBINDBUTTON_H

#include <QPushButton>
#include <QKeyEvent>

class KeybindButton : public QPushButton
{
    Q_OBJECT
public:
    explicit KeybindButton(int currentKey, QWidget *parent = nullptr);
    void setKey(int qtKey);

signals:
    void keyCaptured(int qtKey);

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;

private:
    int m_key;
    bool m_capturing = false;
    void refreshText();

};

#endif // KEYBINDBUTTON_H
