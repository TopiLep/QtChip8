#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_cycleTimer(new QTimer(this))
{
    ui->setupUi(this);

    ui->displaywidget->setChip8(&m_chip);

    QObject::connect(m_cycleTimer, &QTimer::timeout, this, [this]()
    {
        for (int i = 0; i < 12; ++i)
            m_chip.cycle();
        if(m_chip.isDisplayDirty()) {
            ui->displaywidget->updateFrameBuffer();
            m_chip.setDispalyDirty(false);
        }

    });

    QObject::connect(ui->actionOpen_ROM, &QAction::triggered, this, &MainWindow::openROM);

    m_cycleTimer->start(1000 / 60); //60Hz timer
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openROM()
{
    QString file = QFileDialog::getOpenFileName(
        this,
        "Open CHIP-8 ROM",
        "",
        "CHIP-8 ROM (*.ch8 *.rom);;All Files (*)");

    m_chip.loadROM(file.toStdString());
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
    for (int i = 0; i < 16; ++i)
        if (keyMap[i] == e->key()) m_chip.keyPress(i);
}

void MainWindow::keyReleaseEvent(QKeyEvent *e) {
    for (int i = 0; i < 16; ++i)
        if (keyMap[i] == e->key()) m_chip.keyRelease(i);
}