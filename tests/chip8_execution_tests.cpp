#include "chip8.h"
#include <array>
#include <iostream>
#include <stdexcept>

namespace {
void require(bool value, const char* message)
{
    if (!value) throw std::runtime_error(message);
}
void execute(chip8& emu, uint16_t opcode)
{
    emu.writeMemory(emu.getPC(), opcode >> 8);
    emu.writeMemory(emu.getPC() + 1, opcode & 255);
    emu.cycle();
}
std::array<uint8_t, 16> registers(chip8& emu)
{
    execute(emu, 0xA300);
    execute(emu, 0xFF55);
    std::array<uint8_t, 16> result{};
    std::memcpy(result.data(), emu.getMemory() + 0x300, result.size());
    return result;
}
void compatibility()
{
    for (auto mode : {chip8::Compatibility::Legacy, chip8::Compatibility::Original,
                      chip8::Compatibility::Chip48}) {
        chip8 emu;
        emu.setCompatibility(mode);
        const bool original = mode == chip8::Compatibility::Original;
        execute(emu, 0x6108);
        execute(emu, 0x6203);
        execute(emu, 0x8126);
        auto values = registers(emu);
        require(values[1] == (original ? 1 : 4) && values[15] == (original ? 1 : 0), "Shift source/right flag");
        execute(emu, 0x6101);
        execute(emu, 0x6280);
        execute(emu, 0x812E);
        values = registers(emu);
        require(values[1] == (original ? 0 : 2) && values[15] == (original ? 1 : 0), "Shift source/left flag");
        for (uint16_t op : {0x8121, 0x8122, 0x8123}) {
            execute(emu, 0x6F01);
            execute(emu, op);
            require(registers(emu)[15] == (original ? 0 : 1), "Logic VF behavior");
        }
        execute(emu, 0xA400);
        execute(emu, 0xF255);
        require(emu.getI() == (original ? 0x403 : 0x400), "Store index behavior");
        execute(emu, 0xA400);
        execute(emu, 0xF265);
        require(emu.getI() == (original ? 0x403 : 0x400), "Load index behavior");
        execute(emu, 0x6002);
        execute(emu, 0x6208);
        execute(emu, 0xB234);
        require(emu.getPC() == (mode == chip8::Compatibility::Chip48 ? 0x23c : 0x236), "Jump offset register");
        emu.resetEmulatorState();
        require(emu.getCompatibility() == mode, "Reset preserves selected mode");
        emu.writeMemory(0x400, 0xff);
        emu.writeMemory(0x401, 0xff);
        execute(emu, 0xA400);
        execute(emu, 0x603F);
        execute(emu, 0x611F);
        execute(emu, 0xD012);
        const auto* display = emu.getDisplayBuffer();
        require(display[31 * 64 + 63] == 1, "Draw visible edge pixel");
        require(display[31 * 64] == (mode == chip8::Compatibility::Legacy), "Horizontal wrapping");
        require(display[63] == (mode == chip8::Compatibility::Legacy), "Vertical wrapping");
        execute(emu, 0xD012);
        require(registers(emu)[15] == 1, "Draw collision");
    }
}
void flags()
{
    // VF may be an input, output, or both. Check every register alias combination.
    for (int x : {1, 15}) for (int y : {2, 15}) {
        for (int a : {0, 1, 127, 128, 255}) for (int b : {0, 1, 127, 128, 255}) {
            for (int op : {4, 5, 7, 6, 14}) {
                chip8 emu;
                execute(emu, 0x6000 | (x << 8) | a);
                execute(emu, 0x6000 | (y << 8) | b);
                const int left = x == y ? b : a;
                int result = 0, flag = 0;
                switch (op) {
                case 4: result = left + b; flag = result > 255; break;
                case 5: result = left - b; flag = left >= b; break;
                case 7: result = b - left; flag = b >= left; break;
                case 6: result = left >> 1; flag = left & 1; break;
                case 14: result = left << 1; flag = left >> 7; break;
                }
                execute(emu, 0x8000 | (x << 8) | (y << 4) | op);
                const auto values = registers(emu);
                require(values[15] == flag, "Arithmetic flag with VF alias");
                if (x != 15) require(values[x] == static_cast<uint8_t>(result), "Arithmetic result");
            }
        }
    }
    chip8 emu;
    emu.writeMemory(0x400, 0xc0);
    execute(emu, 0xA400);
    execute(emu, 0x6F05);
    execute(emu, 0xDFF1);
    require(emu.getDisplayBuffer()[5 * 64 + 5] == 1 && emu.getDisplayBuffer()[5 * 64 + 6] == 1,
            "Drawing captures VF coordinates before clearing collision flag");
}
void boundaries()
{
    chip8 emu;
    execute(emu, 0x1FFF);
    emu.cycle();
    require(emu.isHalted() && emu.getPC() == 4095, "Invalid instruction fetch halts");
    emu.resetEmulatorState();
    require(!emu.isHalted(), "Reset clears error");
    execute(emu, 0x00EE);
    require(emu.isHalted() && emu.getPC() == 0x200, "Empty stack return halts at instruction");
    emu.resetEmulatorState();
    for (int i = 0; i < 16; ++i) execute(emu, 0x2200);
    require(!emu.isHalted(), "All 16 stack entries usable");
    execute(emu, 0x2200);
    require(emu.isHalted(), "Stack overflow halts");
    for (uint16_t opcode : {0xF033, 0xF155, 0xF165, 0xD012}) {
        emu.resetEmulatorState();
        execute(emu, 0x6005);
        execute(emu, 0xF018);
        execute(emu, 0xAFFF);
        emu.writeMemory(4095, 0xab);
        const auto pc = emu.getPC();
        execute(emu, opcode);
        require(emu.isHalted() && emu.getPC() == pc, "Out-of-range memory operation halts");
        require(emu.getMemory()[4095] == 0xab, "Invalid operation has no partial memory write");
        require(emu.getSoundTimer() == 0, "Fault silences sound");
        emu.cycle();
        require(emu.getPC() == pc, "Halted CPU cannot advance");
    }
    emu.resetEmulatorState();
    emu.writeMemory(-1, 1);
    emu.writeMemory(4096, 1);
    execute(emu, 0x60FF);
    auto pc = emu.getPC();
    execute(emu, 0xE09E);
    require(emu.getPC() == pc + 2, "Invalid key is not pressed");
    pc = emu.getPC();
    execute(emu, 0xE0A1);
    require(emu.getPC() == pc + 4, "Invalid key skip is safe");
    emu.resetEmulatorState();
    execute(emu, 0x5011);
    require(emu.getPC() == 0x202, "Malformed equality opcode does not skip");
    execute(emu, 0x6101);
    pc = emu.getPC();
    execute(emu, 0x9011);
    require(emu.getPC() == pc + 2, "Malformed inequality opcode does not skip");
    execute(emu, 0x01EE);
    require(!emu.isHalted(), "SYS must not decode as return");
    execute(emu, 0x6003);
    emu.keyPress(3);
    emu.releaseKeys();
    pc = emu.getPC();
    execute(emu, 0xF00A);
    require(emu.getPC() == pc, "Released input no longer satisfies key wait");
}
}
int main()
{
    try {
        compatibility();
        flags();
        boundaries();
        std::cout << "All execution regression tests passed\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
