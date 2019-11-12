// Bus Raider Hardware RC2014 512K RAM/ROM
// Rob Dobson 2019

#include "Hw1MBRamRom.h"
#include "../TargetBus/BusAccess.h"
#include "../TargetBus/TargetState.h"
#include "../System/rdutils.h"
#include "../System/logging.h"
#include <stdlib.h>
#include <string.h>

const char* Hw1MBRamRom::_logPrefix = "RC2014_512K_RAM_ROM";
const char* Hw1MBRamRom::_baseName = "512KRAMROM";

static const int Hw1MBRamRom_BASE_ADDR = 0x78;
static const int Hw1MBRamRom_PAGE_ENABLE = 0x7c;

// Value to write to 74ls670 registers to disable the bank
// Actually any value with bit 5 set should work
static const int Hw1MBRamRom_BANK_DISABLE = 0xff;

uint8_t Hw1MBRamRom::_pageOutAllBanks[] = {
    Hw1MBRamRom_BANK_DISABLE,
    Hw1MBRamRom_BANK_DISABLE,
    Hw1MBRamRom_BANK_DISABLE,
    Hw1MBRamRom_BANK_DISABLE
};

Hw1MBRamRom::Hw1MBRamRom() : HwBase()
{
    hwReset();
    _pName = _baseName;
    _memoryEmulationMode = false;
}

// Page out RAM/ROM due to emulation
void Hw1MBRamRom::setMemoryEmulationMode(bool pageOut)
{
    _memoryEmulationMode = pageOut;
}

// Page out RAM/ROM for opcode injection
void Hw1MBRamRom::pageOutForInjection(bool pageOut)
{
    // Check enabled
    if (!_pagingEnabled)
        return;

    // Check page out or restore
    if (pageOut)
    {
        // Write pageOut value
        BusAccess::blockWrite(Hw1MBRamRom_BASE_ADDR, _pageOutAllBanks,
                NUM_BANKS, false, true);
    }
    else
    {
        // Restore
        BusAccess::blockWrite(Hw1MBRamRom_BASE_ADDR, _bankRegisters,
                NUM_BANKS, false, true);
    }
}

// Hardware reset has occurred
void Hw1MBRamRom::hwReset()
{
    _pagingEnabled = true;
    for (int i = 0; i < NUM_BANKS; i++)
        _bankRegisters[i] = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callbacks/Hooks
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Handle a completed bus action
void Hw1MBRamRom::handleBusActionComplete([[maybe_unused]]BR_BUS_ACTION actionType, [[maybe_unused]] BR_BUS_ACTION_REASON reason)
{
    switch(actionType)
    {
        case BR_BUS_ACTION_RESET:
            hwReset();
            break;
        case BR_BUS_ACTION_PAGE_OUT_FOR_INJECT:
            pageOutForInjection(true);
            break;
        case BR_BUS_ACTION_PAGE_IN_FOR_INJECT:
            pageOutForInjection(false);
            break;
        case BR_BUS_ACTION_BUSRQ:
            if (reason == BR_BUS_ACTION_MIRROR)
            {
                // TODO
                // Make a copy of the enire memory while we have the chance
                // int blockReadResult = BusAccess::blockRead(0, getMirrorMemory(), _mirrorMemoryLen, false, false);
                // LogWrite(_logPrefix, LOG_DEBUG, "mirror memory blockRead %s", (blockReadResult == BR_OK) ? "OK" : "FAIL");
            }
            break;
        default:
            break;
    }
}

// Handle a request for memory or IO - or possibly something like in interrupt vector in Z80
void Hw1MBRamRom::handleMemOrIOReq([[maybe_unused]] uint32_t addr, [[maybe_unused]] uint32_t data, 
            [[maybe_unused]] uint32_t flags, [[maybe_unused]] uint32_t& retVal)
{
    // Check for address range used by this card
    if (_pagingEnabled && ((addr & 0xff) >= Hw1MBRamRom_BASE_ADDR) && ((addr & 0xff) < Hw1MBRamRom_BASE_ADDR + NUM_BANKS))
    {
        if(flags & BR_CTRL_BUS_WR_MASK)
        {
            _bankRegisters[(addr & 0xff) - Hw1MBRamRom_BASE_ADDR] = data;
            //TODO
            // ISR_VALUE(ISR_ASSERT_CODE_DEBUG_B + (addr & 0xff) - Hw1MBRamRom_BASE_ADDR, data);
        }
    }
    else if ((addr & 0xff) == Hw1MBRamRom_PAGE_ENABLE)
    {
        if (flags & BR_CTRL_BUS_WR_MASK)
        {
            _pagingEnabled = ((data & 0x01) != 0);
        }
    }
}