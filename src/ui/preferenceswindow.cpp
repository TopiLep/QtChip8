#include "preferenceswindow.h"
#include "ui_preferenceswindow.h"
#include <QSettings>
#include <QLabel>

PreferencesWindow::PreferencesWindow(InputMap *inputMap, chip8 *emulator, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PreferencesWindow)
    , m_inputMap(inputMap)
    , m_emulator(emulator)
{
    ui->setupUi(this);

    setWindowTitle("Preferences");

    QObject::connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);

    QListWidget *categories = ui->listWidget;

    categories->addItem("Controls");
    categories->addItem("Display");
    categories->addItem("Emulation");
    categories->addItem("Audio");

    QObject::connect(ui->listWidget, &QListWidget::currentRowChanged, this, &PreferencesWindow::onCategoryChanged);
    categories->setCurrentRow(0);

    //init controls page
    populateKeybindTable();
    updateResetButton();

    //init display page
    initializeDisplayPage();
    initializeEmulationPage();

}

PreferencesWindow::~PreferencesWindow()
{
    delete ui;
}

void PreferencesWindow::onCategoryChanged(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
    updateResetButton();

    switch (index) {
    case 0:
        // Controls
        populateKeybindTable();
        updateResetButton();
        break;

    case 1:
        // Display
        break;

    case 2:
        // Emulation
        break;

    case 3:
        // Audio
        break;
    }
}

void PreferencesWindow::populateKeybindTable()
{
    if (!m_inputMap)
        return;

    QTableWidget *table = ui->KeybindTable;
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels({"Action", "Key"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setVisible(false);
    table->setRowCount((int)Action::Count);

    for (int i = 0; i < (int)Action::Count; ++i) {
        Action a = (Action)i;

        QTableWidgetItem *nameItem = new QTableWidgetItem(m_inputMap->actionName(a));
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        table->setItem(i, 0, nameItem);

        KeybindButton *btn = new KeybindButton(m_inputMap->getBinding(a), table);
        QObject::connect(btn, &KeybindButton::keyCaptured, this, [this, a](int qtKey) {
            m_inputMap->bind(a, qtKey);
            updateResetButton();
        });
        table->setCellWidget(i, 1, btn);

    }
}

bool PreferencesWindow::currentPageIsModified()
{
    switch (ui->listWidget->currentRow()) {
    case 0:
        return m_inputMap->isModified();

    case 2:
        return m_emulator->getCompatibility() != chip8::Compatibility::Legacy;

    case 1:
    case 3:


    default:
        return false;
    }
}

void PreferencesWindow::resetCurrentPage()
{
    switch (ui->listWidget->currentRow()) {
    case 0:
        m_inputMap->setDefaults();
        populateKeybindTable();
        break;

    case 1:
        break;

    case 2:
        m_compatibility->setCurrentIndex(0);
        break;

    case 3:
        break;
    }

    updateResetButton();
}

void PreferencesWindow::updateResetButton()
{
    bool shouldShow = currentPageIsModified();

    if (shouldShow && !m_resetButton) {
        m_resetButton = ui->buttonBox->addButton("Reset to Default", QDialogButtonBox::ResetRole);
        QObject::connect(m_resetButton, &QPushButton::clicked, this, &PreferencesWindow::resetCurrentPage);
    } else if (!shouldShow && m_resetButton) {
        ui->buttonBox->removeButton(m_resetButton);
        m_resetButton->deleteLater();
        m_resetButton = nullptr;
    }
}

void PreferencesWindow::initializeDisplayPage()
{
    auto* layout = new QFormLayout(ui->DisplayColorButtonContainer);

    m_backgroundColorButton =
        new ColorPickerButton(Qt::black, ui->DisplayColorButtonContainer);

    m_foregroundColorButton =
        new ColorPickerButton(Qt::white, ui->DisplayColorButtonContainer);

    layout->addRow("Primary Color:", m_backgroundColorButton);
    layout->addRow("Secondary Color:", m_foregroundColorButton);
}

void PreferencesWindow::initializeEmulationPage()
{
    auto* layout = new QVBoxLayout(ui->EmulationPage);
    auto* title = new QLabel(tr("Compatibility"), ui->EmulationPage);
    m_compatibility = new QComboBox(ui->EmulationPage);
    m_compatibility->setObjectName("compatibilityMode");
    m_compatibility->addItem(tr("QtChip8 (default)"));
    m_compatibility->addItem(tr("Original CHIP-8 instruction behavior"));
    m_compatibility->addItem(tr("CHIP-48 instruction behavior"));
    m_compatibility->setCurrentIndex(static_cast<int>(m_emulator->getCompatibility()));
    auto* description = new QLabel(tr(
        "Choose a mode if a game's graphics or movement behave incorrectly. "
        "This changes shifts, register transfers, jumps, sprite edges and logic flags. "
        "Changes apply immediately; reload the ROM for a fresh start. "
        "Use the same mode when restoring a saved game. "
        "Extended SUPER-CHIP instructions are not supported."), ui->EmulationPage);
    description->setWordWrap(true);
    layout->addWidget(title);
    layout->addWidget(m_compatibility);
    layout->addWidget(description);
    layout->addStretch();
    connect(m_compatibility, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_emulator->setCompatibility(static_cast<chip8::Compatibility>(index));
        QSettings settings;
        settings.setValue("emulation/compatibility", index);
        updateResetButton();
    });
}
