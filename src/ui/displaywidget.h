#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QWidget>
#include <chip8.h>
#include <QPainter>
#include <QPaintEvent>

class DisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DisplayWidget(QWidget *parent = nullptr);

    void setChip8(chip8* chip);
    void updateFrameBuffer();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    chip8* m_chip = nullptr;
    QImage m_image;

signals:
};

#endif // DISPLAYWIDGET_H
