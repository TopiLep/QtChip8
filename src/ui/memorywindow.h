#ifndef MEMORYWINDOW_H
#define MEMORYWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MemoryWindow;
}
QT_END_NAMESPACE

class MemoryWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MemoryWindow(QWidget *parent = nullptr);
    ~MemoryWindow();

    void setMemory(const uint8_t *memory, uint16_t pc, uint16_t I);

private:
    Ui::MemoryWindow *ui;
};

#endif