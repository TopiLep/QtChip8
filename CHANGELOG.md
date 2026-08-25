# Changelog

All notable changes to this project will be documented in this file.
 
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- ROM file drag-and-drop support
- Error dialog on rom loading when loading failed
- Save/load emulator state ('.c8sav' files) via Emulator -> Save state/ Load state
    - Saves PC, I, SP, V registers, stack, timers, memory and display buffer
- Preferences window with category list (Controls, Display, Emulation, Audio)
- Remappable controls via Preferences -> Controls
    - Click a keybind button to capture and rebind any action to a new key
    - "Reset to Default" button appears when bindings differ from defaults
    - Save and load keybinds from QSettings 
- Added a color picker button with a color selection dialog for display colors.

### Changed

### Fixed
- Rom unloading wouldn't clear display

## [0.2.0]

### Added
- Help -> About dialog with logo, version, license link, and "About Qt" button
- Qt/LGPLv3 attribution in About dialog
- Editable memory in the memory viewer
- Highlight toggles for PC and Index register in the memory viewer

### Changed
- Moved `sounds/` into `assets/sounds/`, added `assets/images/` for the logo
- Version string now generated automatically from `git describe` via CMake
- Switched to Fusion widget style
 
### Fixed
- Sound playback broken after asset folder restructure (stale `qrc:/beep.wav` path)
- Memory update would overwrite while mid memory edit typing

---
