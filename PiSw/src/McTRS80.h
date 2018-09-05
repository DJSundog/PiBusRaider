// Bus Raider Machine TRS80
// Rob Dobson 2018

#include "McBase.h"
#include "McManager.h"
#include "ee_printf.h"

class McTRS80 : public McBase
{
  private:
    static constexpr uint32_t TRS80_KEYBOARD_ADDR = 0x3800;
    static constexpr uint32_t TRS80_KEYBOARD_RAM_SIZE = 0x0100;
    static constexpr uint32_t TRS80_DISP_RAM_ADDR = 0x3c00;
    static constexpr uint32_t TRS80_DISP_RAM_SIZE = 0x400;
    uint8_t _screenBuffer[TRS80_DISP_RAM_SIZE];
    bool _screenBufferValid;
    uint8_t _keyBuffer[TRS80_KEYBOARD_RAM_SIZE];
    bool _keyBufferDirty;

    static McDescriptorTable _descriptorTable;

    static void handleExecAddr(uint32_t execAddr);

  public:

    McTRS80() : McBase()
    {
        // Clear keyboard buffer
        for (uint32_t i = 0; i < TRS80_KEYBOARD_RAM_SIZE; i++)
            _keyBuffer[i] = 0;

        // Ensure keyboard is cleared initially
        _keyBufferDirty = true;

        // Screen buffer invalid
        _screenBufferValid = false;
    }

    // Enable machine
    virtual void enable();

    // Disable machine
    virtual void disable();

    // Get descriptor table for the machine
    virtual McDescriptorTable* getDescriptorTable([[maybe_unused]] int subType)
    {
        return &_descriptorTable;
    }

    // Handle display refresh (called at a rate indicated by the machine's descriptor table)
    virtual void displayRefresh();

    // Handle a key press
    virtual void keyHandler(unsigned char ucModifiers, const unsigned char rawKeys[6]);

    // Handle a file
    virtual void fileHander(const char* pFileInfo, const uint8_t* pFileData, int fileLen);

    // Handle a request for memory or IO - or possibly something like in interrupt vector in Z80
    static uint32_t memoryRequestCallback(uint32_t addr, uint32_t data, uint32_t flags);

};