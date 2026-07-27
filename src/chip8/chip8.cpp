#include "chip8.h"

chip8::chip8()
{

}

void chip8::cycle()
{
    uint16_t opcode = memory[pc];
    opcode = (opcode << 8);
    opcode |= memory[pc + 1];

    uint16_t nib1 = opcode;
    nib1 = nib1 & 0xF000;
    nib1 = nib1 >> 12;

    uint16_t x = opcode;
    x = x & 0x0F00;
    x = x >> 8;

    uint8_t y = opcode;
    y = y & 0x00F0;
    y = y >> 4;

    uint8_t n = opcode;
    n = n & 0x000F;

    uint16_t nnn = opcode;
    nnn = nnn & 0x0FFF;

    uint16_t kk = opcode;
    kk = kk & 0x00FF;

    //printf("PC=0x%03X opcode=0x%04X\n", pc, opcode);
    pc += 2;

    switch (nib1)
    {
    case 0x0:
        switch (kk)
        {
        case 0xE0:
            op_00E0();
            break;

        case 0xEE:
            op_00EE();
            break;

        default:
            // Unknown 0x0 instruction
            break;
        }
        break;

    case 0x1:
        op_1NNN(nnn);
        break;

    case 0x2:
        op_2NNN(nnn);
        break;

    case 0x3:
        op_3XKK(x, kk);
        break;

    case 0x4:
        op_4XKK(x, kk);
        break;

    case 0x5:
        op_5XY0(x, y);
        break;

    case 0x6:
        op_6XKK(x, kk);
        break;

    case 0x7:
        op_7XKK(x, kk);
        break;

    case 0x8:
        switch (n)
        {
        case 0x0: op_8XY0(x, y); break;
        case 0x1: op_8XY1(x, y); break;
        case 0x2: op_8XY2(x, y); break;
        case 0x3: op_8XY3(x, y); break;
        case 0x4: op_8XY4(x, y); break;
        case 0x5: op_8XY5(x, y); break;
        case 0x6: op_8XY6(x); break;
        case 0x7: op_8XY7(x, y); break;
        case 0xE: op_8XYE(x);    break;

        default:
            // Unknown 0x8 instruction
            break;
        }
        break;

    case 0x9:
        op_9XY0(x, y);
        break;

    case 0xA:
        op_ANNN(nnn);
        break;

    case 0xB:
        op_BNNN(nnn);
        break;

    case 0xC:
        op_CXKK(x, kk);
        break;

    case 0xD:
        op_DXYN(x, y, n);
        break;

    case 0xE:
        switch (kk)
        {
        case 0x9E:
            op_EX9E(x);
            break;

        case 0xA1:
            op_EXA1(x);
            break;

        default:
            // Unknown 0xE instruction
            break;
        }
        break;

    case 0xF:
        switch (kk)
        {
        case 0x07: op_FX07(x); break;
        case 0x0A: op_FX0A(x); break;
        case 0x15: op_FX15(x); break;
        case 0x18: op_FX18(x); break;
        case 0x1E: op_FX1E(x); break;
        case 0x29: op_FX29(x); break;
        case 0x33: op_FX33(x); break;
        case 0x55: op_FX55(x); break;
        case 0x65: op_FX65(x); break;

        default:
            // Unknown 0xF instruction
            break;
        }
        break;

    default:
        // Unknown opcode
        break;
    }
}

void chip8::cycleTimers()
{
    if (soundTimer > 0)
        --soundTimer;
    if (delayTimer > 0)
        --delayTimer;
}

bool chip8::loadROM(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        return false;
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size > (4096 - 0x200)) {
        return false;
    }

    resetEmulatorState();
    memset(memory, 0, sizeof(memory));

    file.read(reinterpret_cast<char*>(&memory[0x200]), size);

    const unsigned int FONTSET_START_ADDRESS = 0x50;
    const uint8_t fontset[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
        0x20, 0x60, 0x20, 0x20, 0x70,		// 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
        0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
        0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
        0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
        0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
        0xF0, 0x80, 0xF0, 0x80, 0x80		// F
    };

    for (int i = 0; i < 80; ++i)
        memory[FONTSET_START_ADDRESS + i] = fontset[i];


    return true;
}

const uint8_t *chip8::getDisplayBuffer() const
{
    return &display[0][0];
}

const bool chip8::isDisplayDirty()
{
    return DisplayDirty;
}

void chip8::setDispalyDirty(bool state)
{
    DisplayDirty = state;
}

uint8_t chip8::getSoundTimer() const
{
    return soundTimer;
}

void chip8::keyPress(uint8_t key)
{
    if (key < 16)
        keypad[key] = 1;
}

void chip8::keyRelease(uint8_t key)
{
    if (key < 16)
        keypad[key] = 0;
}

void chip8::updateTimer()
{
    if (delayTimer > 0)
        delayTimer--;
    if (soundTimer > 0)
        soundTimer--;
}

void chip8::resetEmulatorState()
{
    pc = 0x200;
    sp = 0;
    I = 0;
    memset(V, 0, sizeof(V));
    memset(stack, 0, sizeof(stack));
    memset(display, 0, sizeof(display));
    memset(keypad, 0, sizeof(keypad));
    delayTimer = 0;
    soundTimer = 0;

    DisplayDirty = true;
}

const uint8_t *chip8::getMemory() const
{
    return memory;
}

uint16_t chip8::getPC() const
{
    return pc;
}

uint16_t chip8::getI() const
{
    return I;
}

void chip8::op_00E0()
{
    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            display[y][x] = 0;
        }
    }
    DisplayDirty = true;
}

void chip8::op_00EE()
{
    if (sp > 0) {
        sp--;
        pc = stack[sp];
    }
}

void chip8::op_1NNN(uint16_t nnn)
{
    pc = nnn;
}

void chip8::op_2NNN(uint16_t nnn)
{
    if (sp < 16) {
        stack[sp] = pc;
        sp++;
    }
    pc = nnn;
}

void chip8::op_3XKK(uint8_t x, uint8_t kk)
{
    if (V[x] == kk) {
        pc += 2;
    }
}

void chip8::op_4XKK(uint8_t x, uint8_t kk)
{
    if (V[x] != kk) {
        pc += 2;
    }
}

void chip8::op_5XY0(uint8_t x, uint8_t y)
{
    if (V[x] == V[y]) {
        pc += 2;
    }
}

void chip8::op_6XKK(uint8_t x, uint8_t kk)
{
    V[x] = kk;
}

void chip8::op_7XKK(uint8_t x, uint8_t kk)
{
    V[x] += kk;
}

void chip8::op_8XY0(uint8_t x, uint8_t y)
{
    V[x] = V[y];
}

void chip8::op_8XY1(uint8_t x, uint8_t y)
{
    V[x] = (V[x] | V[y]);
}

void chip8::op_8XY2(uint8_t x, uint8_t y)
{
    V[x] = (V[x] & V[y]);
}

void chip8::op_8XY3(uint8_t x, uint8_t y)
{
    V[x] = (V[x] ^ V[y]);
}

void chip8::op_8XY4(uint8_t x, uint8_t y)
{
    uint16_t combination = V[x] + V[y];
    if (combination > 255) {
        V[15] = 1;
    }
    else {
        V[15] = 0;
    }
    V[x] = combination;
}

void chip8::op_8XY5(uint8_t x, uint8_t y)
{
    uint8_t flag = (V[x] >= V[y]) ? 1 : 0;
    V[x] -= V[y];
    V[15] = flag;
}

void chip8::op_8XY6(uint8_t x)
{
    V[15] = (V[x] & 0x1);
    V[x] >>= 1;
}

void chip8::op_8XY7(uint8_t x, uint8_t y)
{
    uint8_t flag = (V[y] >= V[x]) ? 1 : 0;
    V[x] = V[y] - V[x];
    V[15] = flag;
}

void chip8::op_8XYE(uint8_t x)
{
    V[15] = (V[x] & 0x80) >> 7;
    V[x] <<= 1;
}

void chip8::op_9XY0(uint8_t x, uint8_t y)
{
    if (V[x] != V[y]) {
        pc += 2;
    }
}

void chip8::op_ANNN(uint16_t nnn)
{
    I = nnn;
}

void chip8::op_BNNN(uint16_t nnn)
{
    pc = nnn + V[0];
}

void chip8::op_CXKK(uint8_t x, uint8_t kk)
{
    V[x] = (rand() % 256) & kk;
}

void chip8::op_DXYN(uint8_t x, uint8_t y, uint8_t n)
{
    printf("DXYN x=%d y=%d n=%d I=0x%X\n", V[x], V[y], n, I);
    V[15] = 0;
    for (int Y = 0; Y < n; Y++) {
        uint8_t byte = memory[I + Y];
        for (int X = 0; X < 8; X++) {
            if ((byte & (0x80 >> X)) != 0) {
                uint8_t displayX = (X + V[x]) % 64;
                uint8_t displayY = (Y + V[y]) % 32;
                if (display[displayY][displayX] == 1) {
                    V[15] = 1;
                }
                display[displayY][displayX] ^= 1;
            }
        }
    }

    DisplayDirty = true;
}

void chip8::op_EX9E(uint8_t x)
{
    if (keypad[V[x]] == 1) {
        pc += 2;
    }
}

void chip8::op_EXA1(uint8_t x)
{
    if (keypad[V[x]] != 1) {
        pc += 2;
    }
}

void chip8::op_FX07(uint8_t x)
{
    V[x] = delayTimer;
}

void chip8::op_FX0A(uint8_t x)
{
    for (int i = 0; i < 16; i++) {
        if (keypad[i] == 1) {
            V[x] = i;
            return;
        }
    }
    pc -= 2;
}

void chip8::op_FX15(uint8_t x)
{
    delayTimer = V[x];
}

void chip8::op_FX18(uint8_t x)
{
    soundTimer = V[x];
}

void chip8::op_FX1E(uint8_t x)
{
    I = I + V[x];
}

void chip8::op_FX29(uint8_t x)
{
    I = 0x50 + (V[x] * 5);
}

void chip8::op_FX33(uint8_t x)
{
    memory[I] = V[x] / 100;
    memory[I + 1] = V[x] % 100 / 10;
    memory[I + 2] = V[x] % 100 % 10;
}

void chip8::op_FX55(uint8_t x)
{
    for (int i = 0; i <= x; i++) {
        memory[I + i] = V[i];
    }
}

void chip8::op_FX65(uint8_t x)
{
    for (int i = 0; i <= x; i++) {
        V[i] = memory[I + i];
    }
}
