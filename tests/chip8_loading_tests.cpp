#include "chip8.h"

#include <chrono>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool condition, const char* message)
{
    if (!condition) throw std::runtime_error(message);
}

struct TempDirectory {
    std::filesystem::path path = std::filesystem::temp_directory_path() /
        ("qtchip8-tests-" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    TempDirectory() { require(std::filesystem::create_directory(path), "Create test directory"); }
    ~TempDirectory() { std::error_code error; std::filesystem::remove_all(path, error); }
};

using Bytes = std::vector<char>;
void write(const std::filesystem::path& path, const Bytes& data)
{
    std::ofstream file(path, std::ios::binary);
    file.write(data.data(), static_cast<std::streamsize>(data.size()));
    file.close();
    require(file.good(), "Write fixture");
}
Bytes read(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary);
    require(file.good(), "Read fixture");
    return Bytes(std::istreambuf_iterator<char>(file), {});
}
void opcode(chip8& emulator, int address, uint16_t instruction)
{
    emulator.writeMemory(address, instruction >> 8);
    emulator.writeMemory(address + 1, instruction & 0xff);
}
}

int main()
{
    try {
        TempDirectory temp;
        const auto rom = temp.path / "game.ch8";
        const auto state = temp.path / "game.c8sav";
        const auto bad = temp.path / "invalid.c8sav";
        const auto snapshot = temp.path / "snapshot.c8sav";
        chip8 emulator;
        write(rom, {char(0x60), 5, char(0xF0), char(0x18)});
        require(emulator.loadROM(rom), "Load ROM");
        emulator.cycle();
        emulator.cycle();
        opcode(emulator, 0x204, 0xA050);
        opcode(emulator, 0x206, 0xD005);
        emulator.cycle();
        emulator.cycle();
        require(emulator.saveState(state), "Save state");
        const Bytes saved = read(state);
        require(saved.size() == 6203, "Legacy save layout must remain compatible");
        emulator.unloadROM();
        emulator.setDisplayDirty(false);
        emulator.keyPress(3);
        require(emulator.loadState(state), "Restore state");
        require(emulator.isDisplayDirty(), "Restoring must invalidate display");
        require(emulator.getPC() == 0x208 && emulator.getI() == 0x50, "Restore registers");
        require(emulator.getSoundTimer() == 5, "Restore timer without ticking");
        require(emulator.getDisplayBuffer()[5 * 64 + 5] == 1, "Restore pixels");
        require(emulator.saveState(snapshot) && read(snapshot) == saved, "Round trip all serialized fields");
        opcode(emulator, 0x208, 0xF10A);
        emulator.cycle();
        require(emulator.getPC() == 0x208, "Restore clears held keys");
        emulator.keyPress(3);
        emulator.setDisplayDirty(false);
        require(emulator.saveState(snapshot), "Capture current game");
        const Bytes before = read(snapshot);
        auto unchanged = [&] {
            require(emulator.saveState(snapshot) && read(snapshot) == before,
                    "Failed load must preserve entire game");
            require(!emulator.isDisplayDirty(), "Failed load must preserve display status");
        };
        // Exercise every possible truncated save, including partial magic/header fields.
        for (size_t length = 0; length < saved.size(); ++length) {
            write(bad, Bytes(saved.begin(), saved.begin() + length));
            require(!emulator.loadState(bad), "Reject truncated state");
            unchanged();
        }
        auto reject = [&](Bytes bytes) {
            write(bad, bytes);
            require(!emulator.loadState(bad), "Reject invalid state");
            unchanged();
        };
        Bytes bytes = saved;
        bytes[0] ^= 1;
        reject(bytes);
        bytes = saved;
        bytes.push_back(0);
        reject(bytes);
        bytes = saved;
        uint16_t invalidAddress = 4095;
        std::memcpy(bytes.data() + 4, &invalidAddress, sizeof(invalidAddress));
        reject(bytes);
        bytes = saved;
        bytes[8] = 17;
        reject(bytes);
        bytes = saved;
        bytes[8] = 1;
        std::memcpy(bytes.data() + 25, &invalidAddress, sizeof(invalidAddress));
        reject(bytes);
        bytes = saved;
        bytes.back() = 2;
        reject(bytes);
        require(!emulator.loadState(temp.path / "missing"), "Reject missing state");
        unchanged();
        require(!emulator.loadROM(temp.path / "missing"), "Reject missing ROM");
        unchanged();
        write(rom, {});
        require(!emulator.loadROM(rom), "Reject empty ROM");
        unchanged();
        write(rom, Bytes(3585, 1));
        require(!emulator.loadROM(rom), "Reject oversized ROM");
        unchanged();
        emulator.cycle();
        require(emulator.getPC() == 0x20a, "Failed loads preserve held keys");
        write(rom, Bytes(3584, char(0xab)));
        require(emulator.loadROM(rom), "Accept maximum ROM size");
        require(emulator.getMemory()[4095] == 0xab, "Load final ROM byte");
        require(emulator.getMemory()[0x50] == 0xf0, "Install font");
        require(emulator.getPC() == 0x200 && emulator.getSoundTimer() == 0,
                "New ROM resets execution state");
        write(rom, {char(0x12), 0});
        require(emulator.loadROM(rom), "Load shorter ROM");
        require(emulator.getMemory()[0x202] == 0 && emulator.getMemory()[4095] == 0,
                "Shorter ROM clears previous ROM bytes");
        std::cout << "All loading regression tests passed\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
