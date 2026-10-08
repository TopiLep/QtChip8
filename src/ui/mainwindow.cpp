#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include <QSignalBlocker>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_cycleTimer(new QTimer(this))
{
    ui->setupUi(this);
    QSettings settings;
    const int compatibility = settings.value("emulation/compatibility", 0).toInt();
    if (compatibility >= 0 && compatibility <= 2)
        m_chip.setCompatibility(static_cast<chip8::Compatibility>(compatibility));
    connect(qApp, &QApplication::focusChanged, this, [this](QWidget*, QWidget*) {
        m_chip.releaseKeys();
    });
    connect(qApp, &QGuiApplication::applicationStateChanged, this, [this](Qt::ApplicationState state) {
        if (state != Qt::ApplicationActive) m_chip.releaseKeys();
    });

    //load recent roms into the menu
    loadRecentRoms();

    //initialize display
    ui->displaywidget->setChip8(&m_chip);
    ui->displaywidget->setFocusPolicy(Qt::StrongFocus);

    //initialize beeper
    m_beep.setSource(QUrl("qrc:/assets/sounds/beep.wav"));
    m_beep.setLoopCount(QSoundEffect::Infinite);
    m_beep.setVolume(0.1f);

    //timer
    QObject::connect(m_cycleTimer, &QTimer::timeout, this, [this] {
        if (m_state == emulatorState::Running) {
            cycleChip(m_cyclesPerTick);
        }
    });

    //emulator play controls
    QObject::connect(ui->PlayButton, &QToolButton::clicked, this, [this](bool checked) {
        if (checked) {
            m_state = emulatorState::Paused;
            ui->PlayButton->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackPause));
            statusBar()->showMessage("Emulator paused", 3000);
        } else {
            m_state = emulatorState::Running;
            ui->PlayButton->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStart));
            statusBar()->showMessage("Emulator resumed", 3000);
        }
        ui->StepButton->setEnabled(checked);
    });

    QObject::connect(ui->ResetButton, &QToolButton::clicked, this, [this]()
    {
        m_chip.resetEmulatorState();
        ui->displaywidget->updateFrameBuffer();
        statusBar()->showMessage("Emulator state reset", 3000);
    });

    QObject::connect(ui->StepButton, &QToolButton::clicked, this, [this]() {
        cycleChip(1);
    });

    QObject::connect(ui->SpeedComboBox, &QComboBox::currentIndexChanged, this, [this](int index) {
        static const int speeds[] = {1, 3, 6, 12, 24, 48};
        m_cyclesPerTick = (index >= 0 && index < 6) ? speeds[index] : 12;
    });

    //file menu
    QObject::connect(ui->actionOpen_ROM, &QAction::triggered, this, [this]() {
        openROM(openROMPrompt());
    });

    QObject::connect(ui->actionExit, &QAction::triggered, this, [this]() {
        close();
    });

    //emulator menu
    QObject::connect(ui->actionMemory, &QAction::triggered, this, &MainWindow::showMemoryWindow);

    QObject::connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::showPreferencesWindow);

    QObject::connect(ui->actionUnload_ROM, &QAction::triggered, this, [this]() {
        m_chip.unloadROM();
        cycleChip(0);
        m_state = emulatorState::Stopped;

        statusBar()->showMessage("Rom unloaded", 3000);
    });

    QObject::connect(ui->actionSave_state, &QAction::triggered, this, [this]() {
        emulatorState state = m_state;
        m_state = emulatorState::Paused;
        QString path = QFileDialog::getSaveFileName(this, tr("Save State"), QString(), tr("Chip-8 save state (*.c8sav)"));

        if (path.isEmpty()) {
            m_state = state;
            return;
        }

        if (!m_chip.saveState(path.toStdString())) {
            QMessageBox::critical(
                this,
                "Error",
                "Failed to save state:\n" + path
                );
        }

        m_state = state;
    });

    QObject::connect(ui->actionLoad_state, &QAction::triggered, this, [this]() {
        emulatorState state = m_state;
        m_state = emulatorState::Paused;
        QString path = QFileDialog::getOpenFileName(this, tr("Load State"), QString(), tr("Chip-8 Save state (*.c8sav)"));

        if (path.isEmpty()) {
            m_state = state;
            return;
        }

        if (!m_chip.loadState(path.toStdString())) {
            QMessageBox::critical(
                this,
                "Error",
                "Failed to load state:\n" + path
                );
        } else {
            ui->displaywidget->updateFrameBuffer();
            m_chip.setDisplayDirty(false);
            if (m_memoryWindow && m_memoryWindow->isVisible()) {
                m_memoryWindow->setMemory(m_chip.getMemory(), m_chip.getPC(), m_chip.getI());
            }
        }

        m_state = state;
    });

    //help menu
    QObject::connect(ui->actionAbout, &QAction::triggered, this, [this]() {
        aboutDialog dialog(this);
        dialog.exec();
    });

    //start timer
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
    for (int i = 0; i < times && !m_chip.isHalted(); ++i)
        m_chip.cycle();

    if (m_chip.isHalted()) {
        m_state = emulatorState::Paused;
        const QSignalBlocker blocker(ui->PlayButton);
        ui->PlayButton->setChecked(true);
        ui->PlayButton->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackPause));
        ui->StepButton->setEnabled(true);
        statusBar()->showMessage(tr("Emulation stopped at 0x%1: %2")
            .arg(m_chip.getPC(), 3, 16, QLatin1Char('0'))
            .arg(QString::fromUtf8(m_chip.getError())));
    }
    m_chip.cycleTimers();

    if(m_chip.isDisplayDirty()) {
        ui->displaywidget->updateFrameBuffer();
        m_chip.setDisplayDirty(false);
    }

    if (m_memoryWindow && m_memoryWindow->isVisible())
        m_memoryWindow->setMemory(m_chip.getMemory(), m_chip.getPC(), m_chip.getI());

    if (m_chip.getSoundTimer() > 0 && !m_beep.isPlaying()) {
        m_beep.play();
    }
    if (m_chip.getSoundTimer() == 0 && m_beep.isPlaying()) {
        m_beep.stop();
    }
}

void MainWindow::showMemoryWindow()
{
    if (!m_memoryWindow) {
        m_memoryWindow = new MemoryWindow(this);

        connect(m_memoryWindow, &MemoryWindow::memoryEditRequest, this, [this](int address, uint8_t value) {
            m_chip.writeMemory(address, value);
        });
    }

    m_memoryWindow->show();
    m_memoryWindow->raise();
    m_memoryWindow->activateWindow();
    m_memoryWindow->setMemory(m_chip.getMemory(), m_chip.getPC(), m_chip.getI());
}

void MainWindow::showPreferencesWindow()
{
    if (!m_preferencesWindow) {
        m_preferencesWindow = new PreferencesWindow(&m_inputMap, &m_chip, this);
    }

    m_preferencesWindow->show();
    m_preferencesWindow->raise();
    m_preferencesWindow->activateWindow();
    m_preferencesWindow->setModal(false);
}

QString MainWindow::openROMPrompt()
{
    return QFileDialog::getOpenFileName(
               this,
               "Open CHIP-8 ROM",
               "",
               "CHIP-8 ROM (*.ch8 *.rom);;All Files (*)");
}

void MainWindow::openROM(QString file)
{
    if (file.isEmpty()) return;
    if (m_chip.loadROM(file.toStdString())) {
        addRecentRom(file);
        if (m_state == emulatorState::Stopped)
            m_state = emulatorState::Running;
    } else {
        statusBar()->showMessage("Failed to load ROM", 30000);

        QMessageBox::critical(
            this,
            "Error",
            "Failed to open ROM:\n" + file
            );
        return;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
    if (e->isAutoRepeat()) return;
    Action a = m_inputMap.actionForKey(e->key());

    if (a >= Action::Chip8_0 && a <= Action::Chip8_F) {
        m_chip.keyPress((uint8_t)a);
        return;
    }
    if (a == Action::Pause) {
        ui->PlayButton->click();
        return;
    }
    QMainWindow::keyPressEvent(e);
}

void MainWindow::keyReleaseEvent(QKeyEvent *e) {
    if (e->isAutoRepeat()) return;
    Action a = m_inputMap.actionForKey(e->key());

    if (a >= Action::Chip8_0 && a <= Action::Chip8_F) {
        m_chip.keyRelease((uint8_t)a);
        return;
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasUrls())
        return;

    openROM(event->mimeData()->urls().first().toLocalFile());
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (!event->mimeData()->hasUrls())
        return;

    const QUrl url = event->mimeData()->urls().first();

    if (url.isLocalFile() && url.toLocalFile().endsWith(".ch8", Qt::CaseSensitive))
        event->acceptProposedAction();
}
