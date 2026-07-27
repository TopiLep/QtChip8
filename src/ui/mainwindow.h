#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <chip8.h>
#include <qtimer.h>
#include <inputmap.h>
#include <QSettings>
#include <memorywindow.h>

enum class emulatorState {
    Stopped, //No ROM running
    Running, //CPU excecuting instructions
    Paused, //Excecution is stopped
};

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

    //Recent files
    QStringList m_recentRoms;
    void addRecentRom(const QString &file);
    void saveRecentRoms();
    void loadRecentRoms();
    void rebuildRecentMenu();

    //emulator
    chip8 m_chip;
    void cycleChip(int times);
    emulatorState m_state = emulatorState::Stopped;

    //memroy window
    void showMemoryWindow();
    MemoryWindow *m_memoryWindow = nullptr;

    //cycles
    int cyclesPerTick = 12;
    QTimer* m_cycleTimer;

    //keymap
    InputMap m_inputMap;

    //open rom
    QString openROMPromt();
    void openROM(QString file);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
};
#endif // MAINWINDOW_H
