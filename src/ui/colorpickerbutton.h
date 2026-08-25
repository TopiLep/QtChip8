#ifndef COLORPICKERBUTTON_H
#define COLORPICKERBUTTON_H

#include <QPushButton>
#include <QColorDialog>

class ColorPickerButton : public QPushButton
{
public:
    ColorPickerButton(QColor color, QWidget *parent = nullptr);

    QColor color() const;

    void setColor(const QColor& color);

protected:
    void mousePressEvent(QMouseEvent *e) override;

private:
    QColor m_color;
};

#endif // COLORPICKERBUTTON_H
