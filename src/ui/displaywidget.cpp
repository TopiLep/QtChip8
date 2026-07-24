#include "displaywidget.h"

DisplayWidget::DisplayWidget(QWidget *parent)
    : QWidget{parent},
    m_image(64,32, QImage::Format_Indexed8)
{
    m_image.setColorCount(2);
    m_image.setColor(0, qRgb(0, 0, 0));
    m_image.setColor(1, qRgb(255, 255, 255));
    m_image.fill(0);
    setFixedSize(640, 320); // 64*10, 32*10
}

void DisplayWidget::setChip8(chip8 *chip)
{
    m_chip = chip;
}

void DisplayWidget::updateFrameBuffer()
{
    const auto& display = m_chip->getDisplayBuffer();

    for (int y = 0; y < 32; y++) {
        memcpy(m_image.scanLine(y), display + y * 64, 64);
    }
    update();
}

void DisplayWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.drawImage(rect(), m_image);
}
