#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <cstring>

class chip8 {
public:
    chip8();

    void cycle();
    void cycleTimers();
    bool loadROM(const std::filesystem::path& path);

    //display
    const uint8_t* getDisplayBuffer() const;
    bool isDisplayDirty() const;
    void setDisplayDirty(bool state);

    uint8_t getSoundTimer() const;

    void keyPress(uint8_t key);
    void keyRelease(uint8_t key);

    void resetEmulatorState();
    void unloadROM();

    //some getters for memory
    const uint8_t* getMemory() const;
    uint16_t getPC() const;
    uint16_t getI() const;

private:
    //Display
    uint8_t display[32][64]{}; //64x32 display of
    bool displayDirty = false;

    //Memory
    uint8_t memory[4096]{}; //4KB of simulated ram

    //Registers
    uint8_t V[16]{}; //registerit V0-VF
    uint16_t I = 0; //special 16bit register

    //Program counter and stack
    uint16_t pc = 0x200; // starts from 0x200
    uint16_t stack[16]{};
    uint8_t sp = 0;

    //Timers
    uint8_t delayTimer{};
    uint8_t soundTimer{};

    //keypad
    uint8_t keypad[16]{};

    bool stall = false;

    //Op codes\\

    // 0x0***
    void op_00E0();
    void op_00EE();

    // 0x1***
    void op_1NNN(uint16_t nnn);

    // 0x2***
    void op_2NNN(uint16_t nnn);

    // 0x3***
    void op_3XKK(uint8_t x, uint8_t kk);

    // 0x4***
    void op_4XKK(uint8_t x, uint8_t kk);

    // 0x5***
    void op_5XY0(uint8_t x, uint8_t y);

    // 0x6***
    void op_6XKK(uint8_t x, uint8_t kk);

    // 0x7***
    void op_7XKK(uint8_t x, uint8_t kk);

    // 0x8***
    void op_8XY0(uint8_t x, uint8_t y);
    void op_8XY1(uint8_t x, uint8_t y);
    void op_8XY2(uint8_t x, uint8_t y);
    void op_8XY3(uint8_t x, uint8_t y);
    void op_8XY4(uint8_t x, uint8_t y);
    void op_8XY5(uint8_t x, uint8_t y);
    void op_8XY6(uint8_t x);
    void op_8XY7(uint8_t x, uint8_t y);
    void op_8XYE(uint8_t x);

    // 0x9***
    void op_9XY0(uint8_t x, uint8_t y);

    // 0xA***
    void op_ANNN(uint16_t nnn);

    // 0xB***
    void op_BNNN(uint16_t nnn);

    // 0xC***
    void op_CXKK(uint8_t x, uint8_t kk);

    // 0xD***
    void op_DXYN(uint8_t x, uint8_t y, uint8_t n);

    // 0xE***
    void op_EX9E(uint8_t x);
    void op_EXA1(uint8_t x);

    // 0xF***
    void op_FX07(uint8_t x);
    void op_FX0A(uint8_t x);
    void op_FX15(uint8_t x);
    void op_FX18(uint8_t x);
    void op_FX1E(uint8_t x);
    void op_FX29(uint8_t x);
    void op_FX33(uint8_t x);
    void op_FX55(uint8_t x);
    void op_FX65(uint8_t x);


};
#endif // CHIP8_H
