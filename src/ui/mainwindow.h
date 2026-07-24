#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <chip8.h>
#include <qtimer.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;

    //emulator
    chip8 m_chip;

    //timers
    QTimer* m_cycleTimer;

    //keymap
    int keyMap[16] = {
        Qt::Key_X, Qt::Key_1, Qt::Key_2,
        Qt::Key_3, Qt::Key_Q, Qt::Key_W,
        Qt::Key_E, Qt::Key_A, Qt::Key_S,
        Qt::Key_D, Qt::Key_Z, Qt::Key_C,
        Qt::Key_4, Qt::Key_R, Qt::Key_F,
        Qt::Key_V
    };


    void openROM();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
};
#endif // MAINWINDOW_H
