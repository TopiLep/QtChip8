#include "preferenceswindow.h"
#include <QApplication>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QPushButton>
#include <QSettings>
#include <QTemporaryDir>
#include <iostream>
#include <stdexcept>

void require(bool value, const char* message)
{
    if (!value) throw std::runtime_error(message);
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("QtChip8Tests");
    QCoreApplication::setApplicationName("Preferences");
    QTemporaryDir settingsDirectory;
    if (!settingsDirectory.isValid()) return 1;
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDirectory.path());
    try {
        chip8 emu;
        InputMap input;
        PreferencesWindow preferences(&input, &emu);
        auto* categories = preferences.findChild<QListWidget*>("listWidget");
        auto* mode = preferences.findChild<QComboBox*>("compatibilityMode");
        auto* buttons = preferences.findChild<QDialogButtonBox*>("buttonBox");
        require(categories && mode && buttons, "Preferences controls exist");
        categories->setCurrentRow(2);
        require(mode->currentIndex() == 0, "Legacy mode selected initially");
        for (int index : {1, 2}) {
            mode->setCurrentIndex(index);
            require(static_cast<int>(emu.getCompatibility()) == index, "UI applies compatibility to core");
            require(QSettings().value("emulation/compatibility").toInt() == index, "UI persists compatibility");
        }
        QPushButton* reset = nullptr;
        for (auto* button : buttons->buttons()) {
            if (buttons->buttonRole(button) == QDialogButtonBox::ResetRole)
                reset = qobject_cast<QPushButton*>(button);
        }
        require(reset != nullptr, "Modified emulation preferences offer reset");
        reset->click();
        require(mode->currentIndex() == 0 && emu.getCompatibility() == chip8::Compatibility::Legacy,
                "Reset restores legacy behavior");
        require(QSettings().value("emulation/compatibility").toInt() == 0, "Reset is persisted");
        emu.setCompatibility(chip8::Compatibility::Original);
        PreferencesWindow reopened(&input, &emu);
        require(reopened.findChild<QComboBox*>("compatibilityMode")->currentIndex() == 1,
                "Opening preferences reflects active core mode");
        std::cout << "Preferences integration tests passed\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
