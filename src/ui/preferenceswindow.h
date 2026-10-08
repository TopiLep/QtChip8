#ifndef PREFERENCESWINDOW_H
#define PREFERENCESWINDOW_H

#include <QDialog>
#include <QTableWidget>
#include <QFormLayout>

#include "inputmap.h"
#include "chip8.h"
#include <QComboBox>
#include "keybindbutton.h"
#include "colorpickerbutton.h"

namespace Ui {
class PreferencesWindow;
}

class PreferencesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesWindow(InputMap *inputMap, chip8 *emulator, QWidget *parent = nullptr);
    ~PreferencesWindow();

private:

    void onCategoryChanged(int index);

    InputMap *m_inputMap;
    chip8 *m_emulator;
    QComboBox *m_compatibility = nullptr;
    void initializeEmulationPage();
    void populateKeybindTable();

    bool currentPageIsModified();
    void resetCurrentPage();

    QPushButton *m_resetButton = nullptr;
    void updateResetButton();

    ColorPickerButton* m_foregroundColorButton;
    ColorPickerButton* m_backgroundColorButton;
    void initializeDisplayPage();

    Ui::PreferencesWindow *ui;
};

#endif // PREFERENCESWINDOW_H
