from SimpleHDLC import HDLC
import serial
import logging
import os
import time

logging.basicConfig(level=os.environ.get("LOGLEVEL", "INFO"))

romData = []
try:
    with open(r"../../TargetSW/TRS80/ROMS/level1.rom", "rb") as romFile:
        romData = romFile.read()
except:
    print("Not found ... trying alternate")

if len(romData) == 0:
    try:
        with open(r"TargetSW/TRS80/ROMS/level1.rom", "rb") as romFile:
            romData = romFile.read()
    except:
        print("Not found there either")
        exit()

romFrame = bytearray(b"{\"cmdName\":\"filetarget\",\"fileName\":\"level1.rom\"}\0")
romFrame += romData

clearFrame = b"{\"cmdName\":\"cleartarget\"}\0"
resetFrame = b"{\"cmdName\":\"resettarget\"}\0"
progFrame = b"{\"cmdName\":\"programtarget\"}\0"
ioclearFrame = b"{\"cmdName\":\"ioclrtarget\"}\0"
setMCFrame = b"{\"cmdName\":\"setmachine=TRS80\"}\0"

with serial.Serial('COM6', 115200) as s:
    h = HDLC(s)
    h.sendFrame(setMCFrame)
    print("Sent setmachine=TRS80 len", len(setMCFrame))
    time.sleep(2.0)
    h.sendFrame(clearFrame)
    print("Sent cleartarget len", len(clearFrame))
    time.sleep(1.0)
    # h.sendFrame(ioclearFrame)
    # print("Sent ioclear len", len(ioclearFrame))
    # time.sleep(1.0)
    h.sendFrame(romFrame)
    print("Sent ROM srcs len", len(romFrame))
    time.sleep(0.1)
    h.sendFrame(progFrame)
    print("Sent progtarget len", len(progFrame))
    time.sleep(0.1)
    h.sendFrame(resetFrame)
    print("Sent resettarget len", len(resetFrame))