// Bus Raider
// Rob Dobson 2019

#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "../Machines/McBase.h"
#include "../TargetBus/MemorySystem.h"

class RAMEmulator
{
public:
    // Setup
    static void setup(McBase* pTargetMachine);

    // Service
    static void service();

    // Activate emulation
    static void activateEmulation(bool active);
    static bool isActive()
    {
        return _emulationActive;
    }

    // Interrupt handler
    static uint32_t handleWaitInterrupt([[maybe_unused]] uint32_t addr, [[maybe_unused]] uint32_t data, 
            [[maybe_unused]] uint32_t flags, [[maybe_unused]] uint32_t retVal);

    // Memory access
    static uint8_t getMemoryByte(uint32_t addr);
    static uint16_t getMemoryWord(uint32_t addr);
    static void clearMemory();
    static bool blockWrite(uint32_t addr, const uint8_t* pBuf, uint32_t len);
    static bool blockRead(uint32_t addr, uint8_t* pBuf, uint32_t len);

    // Direct access
    static uint8_t* getMemBuffer();
    static int getMemBufferLen();

private:

    // RAM Emulation active
    static bool _emulationActive;

    // Memory system
    static MemorySystem _memorySystem;

    // Target machine
    static McBase* _pTargetMachine;
};