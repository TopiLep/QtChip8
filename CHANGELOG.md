# Changelog

All notable changes to this project will be documented in this file.
 
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

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

---