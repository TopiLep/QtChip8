#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_cycleTimer(new QTimer(this))
{
    ui->setupUi(this);

    loadRecentRoms();

    ui->displaywidget->setChip8(&m_chip);
    ui->displaywidget->setFocusPolicy(Qt::StrongFocus);

    //initialize beeper
    beep.setSource(QUrl("qrc:/sounds/beep.wav"));
    beep.setLoopCount(QSoundEffect::Infinite);
    beep.setVolume(0.1f);

    QObject::connect(m_cycleTimer, &QTimer::timeout, this, [this] {
        if (m_state == emulatorState::Running) {
            cycleChip(cyclesPerTick);
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
        m_chip.resetEmulatorState();
        ui->displaywidget->updateFrameBuffer();
    });

    QObject::connect(ui->StepButton, &QToolButton::clicked, this, [this]() {
        cycleChip(1);
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

    QObject::connect(ui->actionOpen_ROM, &QAction::triggered, this, [this]() {
        openROM(openROMPromt());
    });

    QObject::connect(ui->actionMemory, &QAction::triggered, this, &MainWindow::showMemoryWindow);

    m_cycleTimer->start(1000 / 60); //60Hz timer
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addRecentRom(const QString &file)
{
    m_recentRoms.removeAll(file);
    m_recentRoms.prepend(file);

    while (m_recentRoms.size() > 10)
        m_recentRoms.removeLast();

    saveRecentRoms();
    rebuildRecentMenu();
}

void MainWindow::saveRecentRoms()
{
    QSettings settings("io.github.topilep", "Chip8Emulator");

    settings.setValue("recentFiles/list", m_recentRoms);
}

void MainWindow::loadRecentRoms()
{
    QSettings settings("io.github.topilep", "Chip8Emulator");
    m_recentRoms = settings.value("recentFiles/list").toStringList();

    rebuildRecentMenu();
}

void MainWindow::rebuildRecentMenu()
{
    ui->menuOpen_recent->clear();

    for(const QString &path : std::as_const(m_recentRoms))
    {
        if (!QFile::exists(path))
            continue;

        QAction *action = ui->menuOpen_recent->addAction(QFileInfo(path).fileName());

        action->setData(path);

        connect(action, &QAction::triggered, this, [this, action]() {
            openROM(action->data().toString());
        });
    }
}

void MainWindow::cycleChip(int times)
{
    for (int i = 0; i < times; ++i)
        m_chip.cycle();

    m_chip.cycleTimers();

    if(m_chip.isDisplayDirty()) {
        ui->displaywidget->updateFrameBuffer();
        m_chip.setDispalyDirty(false);
    }

    if (m_memoryWindow && m_memoryWindow->isVisible())
        m_memoryWindow->setMemory(m_chip.getMemory(), m_chip.getPC(), m_chip.getI());

    if (m_chip.getSoundTimer() > 0 && !beep.isPlaying()) {
        beep.play();
    }
    if (m_chip.getSoundTimer() == 0 && beep.isPlaying()) {
        beep.stop();
    }
}

void MainWindow::showMemoryWindow()
{
    if (!m_memoryWindow)
        m_memoryWindow = new MemoryWindow(this);

    m_memoryWindow->show();
    m_memoryWindow->raise();
    m_memoryWindow->activateWindow();
    m_memoryWindow->setMemory(m_chip.getMemory(), m_chip.getPC(), m_chip.getI());
}

QString MainWindow::openROMPromt()
{
    return QFileDialog::getOpenFileName(
               this,
               "Open CHIP-8 ROM",
               "",
               "CHIP-8 ROM (*.ch8 *.rom);;All Files (*)");
}

void MainWindow::openROM(QString file)
{   
    if (m_chip.loadROM(file.toStdString())) {
        addRecentRom(file);
        if (m_state == emulatorState::Stopped)
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