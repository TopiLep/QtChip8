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

    QObject::connect(m_cycleTimer, &QTimer::timeout, this, [this] {
        if (m_state == emulatorState::Running) {
            cycleChip();
        }
    });

    QObject::connect(ui->PlayButton, &QToolButton::clicked, this, [this](bool checked) {
        if (checked) {
            m_state = emulatorState::Paused;
            ui->PlayButton->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackPause));
        } else {
            m_state = emulatorState::Running;
            ui->PlayButton->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStart));
        }
        ui->StepButton->setEnabled(checked);
    });

    QObject::connect(ui->ResetButton, &QToolButton::clicked, this, [this]()
    {
        qDebug() << "reset";
        m_chip.resetEmulatorState();
    });

    QObject::connect(ui->StepButton, &QToolButton::clicked, this, [this]() {
        cycleChip();
    });

    QObject::connect(ui->SpeedComboBox, &QComboBox::currentIndexChanged, this, [this](int index) {
        switch (index) {
        case 0:
            cyclesPerTick = 1;
            break;
        case 1:
            cyclesPerTick = 3;
            break;
        case 2:
            cyclesPerTick = 6;
            break;
        case 3:
            cyclesPerTick = 12;
            break;
        case 4:
            cyclesPerTick = 24;
            break;
        case 5:
            cyclesPerTick = 48;
            break;
        default:
            cyclesPerTick = 12;
            break;
        }
    });

    QObject::connect(ui->actionOpen_ROM, &QAction::triggered, this, &MainWindow::openROM);

    m_cycleTimer->start(1000 / 60); //60Hz timer
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::cycleChip()
{
    for (int i = 0; i < cyclesPerTick; ++i)
        m_chip.cycle();
    if(m_chip.isDisplayDirty()) {
        ui->displaywidget->updateFrameBuffer();
        m_chip.setDispalyDirty(false);
    }
}

void MainWindow::openROM()
{   
    QString file = QFileDialog::getOpenFileName(
        this,
        "Open CHIP-8 ROM",
        "",
        "CHIP-8 ROM (*.ch8 *.rom);;All Files (*)");

    if (m_chip.loadROM(file.toStdString())) {
        m_state = emulatorState::Running;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
    Action a = m_inputMap.actionForKey(e->key());

    if (a >= Action::Chip8_0 && a <= Action::Chip8_F) {
        m_chip.keyPress((uint8_t)a);
        return;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *e) {
    Action a = m_inputMap.actionForKey(e->key());

    if (a >= Action::Chip8_0 && a <= Action::Chip8_F) {
        m_chip.keyRelease((uint8_t)a);
        return;
    }
}