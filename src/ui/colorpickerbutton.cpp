#include "colorpickerbutton.h"

ColorPickerButton::ColorPickerButton(QColor color, QWidget *parent)
    : m_color(color),
      QPushButton(parent)
{
    setObjectName("ColorPickerButton");
    setColor(m_color);
}

void ColorPickerButton::mousePressEvent(QMouseEvent *e)
{
    const QColor color = QColorDialog::getColor(m_color, this);

    setColor(color);

    QPushButton::mousePressEvent(e);
}

QColor ColorPickerButton::color() const
{
    return m_color;
}

void ColorPickerButton::setColor(const QColor &color)
{
    if (!color.isValid())
        return;

    m_color = color;

    setStyleSheet(
        QString("#ColorPickerButton { background-color: %1; }")
            .arg(m_color.name())
     );
}

